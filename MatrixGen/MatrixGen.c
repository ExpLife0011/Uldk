// font.cpp : 定义控制台应用程序的入口点。
//

#include <ft2build.h>
#include <windows.h>

#include "MatrixFile.h"
#include FT_FREETYPE_H

static FT_Library Ftl;

static SECTION_NODE SecNodeHead = { 0 };

typedef struct _ENCODE_RANGE
{
	UINT16 Start;
	UINT16 End;
}ENCODE_RANGE, *PENCODE_RANGE;

//only 16*16

FT_UInt
map_mono_to_vector(
	FT_GlyphSlot Slot,
	UCHAR* Vector,
	UINT16 Width,
	UINT16 Height
) {

	if (Slot->format != FT_GLYPH_FORMAT_BITMAP)
		return 0;

	//这里计算反映到16*16矩阵表示的宽度
	FT_UInt real_width = Slot->bitmap_left + Slot->bitmap.width;
	FT_UInt width = ALIGN_UP_BY(real_width, 8);
	FT_UInt real_width_byte = (width == 0 ? 1 : width / 8);

	//这里计算freetype内部数组表示的宽度
	FT_UInt ft_width_byte = ALIGN_UP_BY(Slot->bitmap.width, 8) / 8;

	//这是16*16矩阵跳到下一行经过的像素树
	FT_UInt vwidth_byte = Width / 8;
	//这是freetype内部数组经过的像素数
	FT_UInt line_byte = Slot->bitmap.pitch;

	//这里减2是因为FREETYPE的默认原点在2像素的地方，不知道怎么把他调到0像素
	for (int i = Height - Slot->bitmap_top - 2, j = 0;j < Slot->bitmap.rows;i++, j++)
	{

		memcpy(
			&Vector[vwidth_byte*i],
			&Slot->bitmap.buffer[line_byte*j],
			ft_width_byte
		);

		(*((unsigned short*)&Vector[vwidth_byte*i])) >>= Slot->bitmap_left;

	}

	return real_width_byte;
}

int
translate_vector_to_array(
	UCHAR* Vector,
	FT_UInt real_width_byte,
	FT_UInt width,
	FT_UInt height,
	FT_Bytes pixel_array
) {

	FT_UInt vwidth_byte = width / 8;

	for (FT_UInt i = 0;i < height;i++)
	{

		memcpy((void*)&pixel_array[i*real_width_byte], &Vector[i*vwidth_byte], real_width_byte);

	}

	return real_width_byte*height;

}

/* 初始化Freetype库 */

FT_Face
InitializeFreeType(
	char* FontFile
) {

	int error = FT_Init_FreeType(&Ftl);
	if (error)
		return 0;

	FT_Face Face;
	error = FT_New_Face(Ftl,
		FontFile,
		0,
		&Face);
	if (error)
		return 0;

	error = FT_Set_Pixel_Sizes(
		Face,
		0,
		16);
	if (error)
		return 0;

	return Face;

}

FT_Bytes
AllocWorkMemory(
	SECTION_NODE* SecNodeHead
) {

	FT_UInt32 SectionCount = 0, CharCount = 0;

	SECTION_NODE* Last = SecNodeHead;
	while (Last->Next != NULL) { 
		SectionCount += 1;
		CharCount += (Last->SectionInfo.Last - Last->SectionInfo.First);
		Last = Last->Next; 
	}

	FT_Bytes WorkMem = (FT_Bytes)malloc(
		SectionCount * sizeof(FL_SECTION_INF) + CharCount * (sizeof(UFL_CHAR_INDEX) + 32) + 4096
	);

	return WorkMem;

}

/* 创建编码节 */

void 
CreateFontSection(
	SECTION_NODE* SecNodeHead,
	FT_Int16 Start,
	FT_Int16 End
) {

	SECTION_NODE* Node = (SECTION_NODE*)malloc(sizeof(*Node));
	Node->SectionInfo.First = Start;
	Node->SectionInfo.Last = End;
	Node->CharList = (UFL_CHAR_INDEX*)malloc((End - Start) * sizeof(UFL_CHAR_INDEX));
	Node->Next = 0;
	
	SECTION_NODE* Last = SecNodeHead;
	while (Last->Next != NULL) Last = Last->Next;

	Last->Next = Node;

}

void 
GenerateFontMatrix(
	SECTION_NODE*		Node,
	FT_Face				Face,
	FT_Bytes*			MatrixBuffer,
	FT_Bytes**			NextMatrix,
	FT_Int32			CurOffset
) {

	int error;

	for (FT_UInt16 i = Node->SectionInfo.First, j = 0;i < Node->SectionInfo.Last;i++, j++) {

		/* 加载字莫 */
		error = FT_Load_Char(
			Face,
			i,
			FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO
		);
		/* 32是个特殊情况,空格 */
		if (error || ((Face->glyph->bitmap.rows == 0) && (i != 32))) {
			Node->CharList[i].OffAddr = 0;
			Node->CharList[i].Width = 0;
		}

		unsigned char bitmap[2 * 16] = { 0 };
		/* 把freetype生成的有效点阵转换为完全点阵 */
		int real_width_byte = map_mono_to_vector(Face->glyph, bitmap, 16, 16);

		Node->CharList[i].Width = real_width_byte * 8;
		Node->CharList[i].OffAddr = CurOffset;

		/* 将点阵放到数组中,这里可能会从16*16转为8*16,不是没有必要的 */
		MatrixBuffer += translate_vector_to_array(bitmap, real_width_byte, 16, 16, MatrixBuffer);

		CurOffset += real_width_byte * 16;

	}

	memcpy(MatrixBuffer,
		Node->CharList,
		(Node->SectionInfo.First - Node->SectionInfo.Last) * sizeof(UFL_CHAR_INDEX)
	);

	*NextMatrix = MatrixBuffer;

}

int main(int args,char** argv)
{

	if (args < 3)
		return 0;

	/* CJK in Unicode Range */

	FL_Header Header;
	int error = 0;

	FT_Face Face = InitializeFreeType(argv[1]);
	if (Face == 0)
		return 0;

	/* 未经测试的模块化代码 */

	/* 创建ascii和cjk区域的字符集 */

	CreateFontSection(&SecNodeHead, 0x20, 0xFF);
	CreateFontSection(&SecNodeHead, 0x4E02, 0x9FFF);

	FT_UInt32 CurOffset = 0;
	Header.nSection = 2;

	CurOffset += sizeof(FL_Header);
	SECTION_NODE* Last = &SecNodeHead;
	
	/* 计算索引表开始偏移 */

	while (Last->Next != NULL) {
		CurOffset += sizeof(FL_SECTION_INF);
		Last = Last->Next;
	}

	/* 计算各个索引表偏移 */

	Last = &SecNodeHead;
	while (Last->Next != NULL) {
		Last->SectionInfo.OffAddr = CurOffset;
		CurOffset += (Last->SectionInfo.Last - Last->SectionInfo.First) * sizeof(UFL_CHAR_INDEX);
		Last = Last->Next;
	}

	/* 计算大概需要的内存 */
	FT_Bytes WorkMem = AllocWorkMemory(&SecNodeHead), FirstMetrix = WorkMem, NextMetrix;
	if (WorkMem == NULL)
		return 0;

	/* 填充字模 */

	Last = &SecNodeHead;
	while (Last->Next != NULL) {
		
		GenerateFontMatrix(
			Last,
			Face,
			FirstMetrix,
			&NextMetrix,
			CurOffset
		);
		
		CurOffset = (NextMetrix - FirstMetrix);
		FirstMetrix = NextMetrix;

		Last = Last->Next;
	}

	/* 生成字模文件 */

	FILE* fp = fopen(argv[2], "w+b");
	fwrite(&Header, sizeof(FL_Header), 1, fp);
	
	Last = &SecNodeHead;
	while (Last->Next != NULL) {

		fwrite(&Last->SectionInfo, sizeof(FL_SECTION_INF), 1, fp);
		fwrite(Last->CharList, sizeof(UFL_CHAR_INDEX), Last->SectionInfo.Last - Last->SectionInfo.First, fp);
		free(Last->CharList);

	}

	fwrite(WorkMem, (NextMetrix - WorkMem), 1, fp);
	free(WorkMem);

	fclose(fp);

	return 1;

}

