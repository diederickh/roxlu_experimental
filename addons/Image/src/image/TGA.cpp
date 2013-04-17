#include <fstream>
#include <roxlu/core/Utils.h>
#include <image/TGA.h>

TGA::TGA()
	:pixels(NULL)
	,width(0)
	,height(0)
	,id(0)
	,bits_per_pixel(0)
  ,num_bytes(0)
  ,color_type(TGA_COLOR_TYPE_NONE)
  ,stride(0)
{
}

TGA::TGA(const TGA& other) 
  :pixels(NULL)
{
  clone(other);
}

TGA& TGA::operator=(const TGA& other) {
  return clone(other);
}

TGA& TGA::clone(const TGA& other) {
  if(pixels) {
    delete[] pixels;
  }

  if(other.num_bytes > 0 && other.pixels) {
    pixels = new unsigned char[other.num_bytes];
    memcpy((char*)pixels, (char*)other.pixels, other.num_bytes);
  }

  width = other.width;
  height = other.height;
  bits_per_pixel = other.bits_per_pixel;
  id = other.id;
  num_bytes = other.num_bytes;
  color_type = other.color_type;
  stride = other.stride;

  memcpy(header, other.header, sizeof(header));
  return *this;
}

TGA::~TGA() {
	if(pixels != NULL) {
		delete[] pixels;
    pixels = NULL;
	}
}

bool TGA::save(string filepath, bool datapath) { 
  RX_ERROR("We need to fix this function!!");

	TGAHeader header;
	int color_mode;
	rx_uint8 color_swap;

  if(datapath) {
    filepath = rx_to_data_path(filepath);
  }
	
	std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
	if(!ofs.is_open()) {
		return false;
	}
	
	memset(&header, 0, sizeof(TGAHeader));
	header.bit_count = bits_per_pixel;
	header.color_map_entry_size = 0;
	header.color_map_length = 0;
	header.color_map_origin = 0;
	header.image_descriptor = 0;
	header.image_height = height;
	header.image_width = width;
	header.image_id_length = 0;
	header.image_type_code = 2;
	header.image_x_origin = 0;
	header.image_y_origin = 0;
	
	ofs.write((char*)&header, sizeof(TGAHeader));

	for(int i = 0; i < num_bytes; i += 3) {
		color_swap = pixels[i];
		pixels[i] = pixels[i+2];
		pixels[i+2] = color_swap;
	}

	ofs.write((char*)pixels, num_bytes);
	ofs.close();
	return true;
}


// super naive loading of uncompressed targa images
bool TGA::load(string filepath, bool datapath) {
  if(pixels) {
    delete[] pixels;
  }

  if(datapath) {
    filepath = rx_to_data_path(filepath);
  }

	std::ifstream ifs;
	ifs.open(filepath.c_str(), std::ios::binary | std::ios::ate);
	if(!ifs.is_open()) {
		RX_ERROR("Cannot open: '%s'\n", filepath.c_str());
		return false;
	}
	
	ifs.seekg(0, std::ios::beg);
	ifs.read((char*)header, sizeof(unsigned char)*12);
	ifs.read((char*)&width, sizeof(unsigned short));
	ifs.read((char*)&height, sizeof(unsigned short));
	ifs.read((char*)&bits_per_pixel, sizeof(unsigned char));
	ifs.read((char*)&id, sizeof(unsigned char));
  
  color_type = header[2];
	
	num_bytes = (rx_uint32)width * height * (bits_per_pixel / 8);
	if(pixels != NULL) {
		delete[] pixels;
	}

  num_channels = bits_per_pixel / 8;
  stride = width * num_channels;

	pixels = new unsigned char[num_bytes];
  unsigned char* tmp_pixels = new unsigned char[num_bytes];
	ifs.read((char*)tmp_pixels, num_bytes);

  // FLIP BOTTOM<>TOP
  size_t src_dx = 0;
  size_t dst_dx = 0;
  for(size_t j = 0; j < height; ++j) {
    src_dx = ((height - j) * stride) - stride;
    dst_dx = (j * stride);
    memcpy(pixels + dst_dx, tmp_pixels + src_dx, stride);
  }
  delete[] tmp_pixels;

  // SWAP BGR -> RGB
  unsigned char tmp_col;
  for(size_t j = 0; j < height; ++j) {
    for(size_t i = 0; i < width; ++i) {
      src_dx = j * stride + i * num_channels;
      tmp_col = pixels[src_dx];
      pixels[src_dx] = pixels[src_dx + 2];
      pixels[src_dx + 2] = tmp_col;
    }
  }

	ifs.close();

	return true;
}

void TGA::print() {
	RX_VERBOSE("TGA.header: %s", header);
	RX_VERBOSE("TGA.width: %d", width);
	RX_VERBOSE("TGA.height: %d", height);
	RX_VERBOSE("TGA.bits_per_pixel: %d", bits_per_pixel)
	RX_VERBOSE("TGA.id: %d", id);
	RX_VERBOSE("TGA.num bytes: %zu", num_bytes);	
  RX_VERBOSE("TGA.color_type: %s", colorTypeToString(color_type).c_str());
}

std::string TGA::colorTypeToString(unsigned int t) {
  if(t == TGA_COLOR_TYPE_NONE) {
    return "TGA_COLOR_TYPE_NONE";
  }
  else if(t == TGA_COLOR_TYPE_UNCOMPRESSED_MAPPED) {
    return "TGA_COLOR_TYPE_UNCOMPRESSED_MAPPED";
  }
  else if(t == TGA_COLOR_TYPE_UNCOMPRESSED_RGB) {
    return "TGA_COLOR_TYPE_UNCOMPRESSED_RGB";
  }
  else if(t == TGA_COLOR_TYPE_UNCOMPRESSED_BW ) {
    return "TGA_COLOR_TYPE_UNCOMPRESSED_BW";
  }
  else if(t == TGA_COLOR_TYPE_RLE_MAPPED) {
    return "TGA_COLOR_TYPE_RLE_MAPPED";
  }
  else if(t == TGA_COLOR_TYPE_RLE_RGB) {
    return "TGA_COLOR_TYPE_RLE_RGB";
  }
  else if(t == TGA_COLOR_TYPE_COMPRESSED_BW) {
    return "TGA_COLOR_TYPE_COMPRESSED_BW";
  }
  else if(t == TGA_COLOR_TYPE_COMPRESSED_MAPPED) {
    return "TGA_COLOR_TYPE_COMPRESSED_MAPPED";
  }
  else if(t == TGA_COLOR_TYPE_COMPRESSED_MAPPED_QUAD) {
    return "TGA_COLOR_TYPE_COMPRESSED_MAPPED_QUAD";
  }
  else {
    return "UNKNOWN";
  }
}
