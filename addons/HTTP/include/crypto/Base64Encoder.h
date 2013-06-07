#ifndef __BASE64_H__
#define __BASE64_H__

#include <assert.h>

/*
=============================================================

aoBase64Encoder

Author: Arne Olav Hallingstad
History:
Initial implementation 2011-01-16

Encodes a data stream in base 64 which can be passed along
in strings, used in particular as part of the URI in
HTTP-requests

The base 64 encoded string needs 4 bytes per 3 bytes in the
original data stream + up to 3 bytes in padding at the
end (encoded string must be multiple of 4). Thus encoded size
is roughly 33,3% of the original size.

Based on RFC 3548

=============================================================
*/

class aoBase64Encoder {
public:
	static const char BASE64_PAD_CHAR = '=';

					aoBase64Encoder();
					~aoBase64Encoder();

					// Sets the encoded data for object
					// Frees the previously encoded data if any
	void			Reset( const char* encodedData, unsigned int encodedDataSize );
	void			Reset();
					// Encodes the input and stores it in the object.
					// Frees the previously encoded data if any
	void			Encode( const unsigned char* data, unsigned int dataSize );
					// Decodes and returns the currently encoded data
					// Decoded data is freed when object is destroyed
	bool			Decode( unsigned char*& data, unsigned int& dataSize );
					// Get pointer to the encoded data
	const char*		GetEncoded() const { return encoded; }
					// Get size of encoded data
	unsigned int	GetEncodedSize() const { return encodedSize; }
					// Get pointer to the decoded data
					// decodes encoded data if necessary
	const unsigned char* GetDecoded();
					// Get size of encoded data
					// decodes encoded data if necessary
	unsigned int	GetDecodedSize();

private:
	unsigned int	GetBase64Size( unsigned int dataSize ) { return ( ( dataSize + 2 ) / 3 ) * 4; }
					// this is the padded data size, we 
					// cannot know the original data size unless we also look at the pad characters
	unsigned int	GetDataSize( unsigned int base64Size );

	char*			encoded;
	unsigned int	encodedSize;
	unsigned char*	decoded;
	unsigned int	decodedSize;
};

#endif // !__BASE64_H__
