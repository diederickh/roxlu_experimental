#include "BitmapFont.h"

BitmapFont::BitmapFont() {
	for(int i = 0; i < 256; ++i) {
		u0[i] = 0;
		u1[i] = 0;
		v0[i] = 0;
		v1[i] = 0;
		cw[i] = 0;
	}
	tw = 0;
	th = 0;
	ch = 0;
	buffer = NULL;
	load();
}

BitmapFont::~BitmapFont() {
	if(buffer) {
		delete[] buffer;
	}	
	tw = 0;
	th = 0;
	ch = 0;
}


int BitmapFont::nextPow2(int n) {
	int r = 1;
	while(r < n) {
		r *= 2;
	}
	return r;
}


bool BitmapFont::load() {
	//return generate(small_font, small_font_w, small_font_h);
	return generate(medium_font, medium_font_w, medium_font_h);
}

bool BitmapFont::generate(const unsigned char* src, int imgW, int imgH) {
	// find height of font
	int i,j;
	int h = 0, hh = 0;
	int r, nb_row = 0;
	
	for(j = 0; j < imgH; ++j) {
		if(src[j*imgW] == 0) {
			if( (hh <= 0 && h <= 0) || (h != hh && h > 0 && hh > 0) ) {
				printf("Bad font height.\n");
				return false;
			}
			else if(h <= 0) {
				h = hh;
			}
			else if(hh <= 0) {
				break;
			}
			hh = 0; 
			++nb_row;
		}
		else {
			++hh;
		}
	} 
	

	// find the width and position of each character.
	int w = 0;
	int x0[224], y0[224], x1[224], y1[224];
	int c = 32;
	int start;
	for(r = 0; r < nb_row; ++r) {
		start = 1;
		for(i = 1; i < imgW; ++i) {
			if(src[(r*(h+1)+h) * imgW+i] == 0 || i == imgW-1) {
				if(i == start) {
					break;
				}
				if(c < 256) {
					x0[c-32] = start;
					x1[c-32] = i;
					//printf("w: %d\n", x1[ch-32]-x0[ch-32]);
					y0[c-32] = r * (h+1);
					y1[c-32] = r * (h+1)+h-1;
					w += i - start + 1;
					start = i + 1; 
				}
				++c;
			}
		}
	}
	
	for(i = c-32; i < 224; ++i) {
		x0[c] = 0;
		x1[c] = 0;
		y0[c] = 0;
		y1[c] = 0;
	}
	
	// repack 14 rows of 16 chars.
	int l, lmax = 1;
	for(r = 0; r < 14; ++r) {
		l = 0;
		for(i = 0; i < 16; ++i) {
			l += x1[i+r*16]-x0[i+r*16]+1;
		}
		if(l > lmax) {
			lmax = l;
		}
	}
	
	// add a little marging between characters to avoid anti aliasing artifacts
	const int MARGIN_X = 2;
	const int MARGIN_Y = 2;
	lmax += 16*MARGIN_X;
	
	// build the texture.
	num_chars = c-32;
	ch = h;
	tw = BitmapFont::nextPow2(lmax);
	th = BitmapFont::nextPow2(14*(h+MARGIN_Y));
	buffer = new unsigned char[tw*th];
	memset(buffer, 0, tw*th);
	
	int xx;
	float du = 0.0f;
	float dv = 0.0f;
	float alpha;
	for(r = 0; r < 14; ++r) {
		for(xx = 0, c=r*16; c < (r+1)*16; ++c) {
			if(y1[c] - y0[c] == h-1) {
				for(j = 0; j < h; ++j) {
					for(i = x0[c]; i <= x1[c]; ++i) {
						alpha = ((float)(src[i+(y0[c]+j)*imgW]))/256.0f;
						buffer[(xx+i-x0[c])+(r*(h+MARGIN_Y)+j)*tw] = (unsigned char)(alpha*256.0f);
					}
				}
				u0[c+32] = (float(xx)+du)/float(tw);
				xx += x1[c]-x0[c]+1;
				u1[c+32] = (float(xx)+du)/float(tw);
				v0[c+32] = (float(r*(h+MARGIN_Y))+dv)/float(th);
				v1[c+32] = (float(r*(h+MARGIN_Y)+h)+dv)/float(th);
				cw[c+32] = x1[c]-x0[c]+1;
				//printf("%d\n", cw[c+32]);
				xx += MARGIN_X;
			}
		}
	}
	
	const unsigned char undef = 127;
	for(c = 0; c < 32; ++c) {
		u0[c] = u0[undef];
		u1[c] = u1[undef];
		v0[c] = v0[undef];
		v1[c] = v1[undef];
		cw[c] = cw[undef]/2.0f;
	}

	glEnable(GL_TEXTURE_2D); eglGetError();
	glGenTextures(1, &tex); eglGetError();
	glBindTexture(GL_TEXTURE_2D, tex); eglGetError();
	
	//
//	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
//    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
//    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
//    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
//    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    glPixelTransferf(GL_ALPHA_SCALE, 1);
//    glPixelTransferf(GL_ALPHA_BIAS, 0);
//    glPixelTransferf(GL_RED_BIAS, 1);
//    glPixelTransferf(GL_GREEN_BIAS, 1);
//    glPixelTransferf(GL_BLUE_BIAS, 1);
	//
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); eglGetError(); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); eglGetError();
	glTexImage2D(
		 GL_TEXTURE_2D
		,0
		,GL_LUMINANCE8
		,tw
		,th
		,0
		,GL_LUMINANCE
		,GL_UNSIGNED_BYTE
		,buffer
	); eglGetError();
	
	//
//  glBindTexture(GL_TEXTURE_2D, 0);
//    glPixelTransferf(GL_ALPHA_BIAS, 0);
//    glPixelTransferf(GL_RED_BIAS, 0);
//    glPixelTransferf(GL_GREEN_BIAS, 0);
//    glPixelTransferf(GL_BLUE_BIAS, 0);
	//
//,GL_RGBA
	printf("h = %d, hh = %d, nb_row:%d, w:%d\n", h, hh, nb_row,w);
	
	return true;
}

void BitmapFont::bind() {
	glBindTexture(GL_TEXTURE_2D, tex); eglGetError();
}

void BitmapFont::buildText(TextVertices& vertices, const std::string& text, Color4* color) {
	int x, x1, y, y1, i, len;
	unsigned char c;
	y = 0;
	y1 = y+ch;
	printf("y1: %d\n", y1);
	len = text.length();
	x = 0;
	for(i = 0; i < len; ++i) {
		c = text[i];
		x1 = x + cw[c];
		//printf("x: %f, y:%f\n", x, y);
		//printf("SET: %d, %d, %d\n", x, y, int(c));
		vertices.add(x, y, u0[c], v0[c]);
		vertices.add(x1, y, u1[c], v0[c]);
		vertices.add(x, y1, u0[c], v1[c]);
		
		vertices.add(x1, y, u1[c], v0[c]);
		vertices.add(x1, y1, u1[c], v1[c]);
		vertices.add(x, y1, u0[c], v1[c]);
				
		/*
		tt.text_verts.push_back(Vec2(x,y));
		tt.text_verts.push_back(Vec2(x1,y));
		tt.text_verts.push_back(Vec2(x,y1));
		
		tt.text_verts.push_back(Vec2(x1,y));
		tt.text_verts.push_back(Vec2(x1,y1));
		tt.text_verts.push_back(Vec2(x,y1));
		
		tt.text_uvs.push_back(Vec2(u0[ch], v0[ch]));
		tt.text_uvs.push_back(Vec2(u1[ch], v0[ch]));
		tt.text_uvs.push_back(Vec2(u0[ch], v1[ch]));
		
		tt.text_uvs.push_back(Vec2(u1[ch], v0[ch]));
		tt.text_uvs.push_back(Vec2(u1[ch], v1[ch]));
		tt.text_uvs.push_back(Vec2(u0[ch], v1[ch]));
		*/
		
		x = x1;
	}
	
}


