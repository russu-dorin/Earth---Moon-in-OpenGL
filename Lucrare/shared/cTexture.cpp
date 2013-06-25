#include "cTexture.h"

cTexture::cTexture(void)
{
	texHandle=-1;
	strcpy(texPath,"null");
}

cTexture::~cTexture(void)
{
	//if(texHandle>=0)
	glDeleteTextures(1,&texHandle);
}
void cTexture::LoadFromFile(const char *filePathIn)
{
	strcpy(texPath,filePathIn);
	texHandle = SOIL_load_OGL_texture(
	filePathIn,
    SOIL_LOAD_RGBA,
	SOIL_CREATE_NEW_ID,
	SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y);
}
GLuint cTexture::GetTextureHandle()
{
	return this->texHandle;	
}
const char * cTexture::GetTexturePath()
{
	return texPath;
}

