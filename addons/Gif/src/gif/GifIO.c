/*=========================================================================

 gif-io.c, part of ENIAC. 
 Electrical Network Interactive Console - 2008

 Based around original GIF code by David Koblas.
 Modified by Adam D. Moss.
 Modified by Sven Neumann.
 Modified by Andrea Viarengo.
 
 ========================================================================== 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ==========================================================================*/

/* GIF saving and loading routines stripped out of the GIF filter for The GIMP.
 *
 * Modified for Pipmak game Engine by Andrea Viarengo in 2008 <andrea.viarengo@gmail.com>
 * Modified for blinkentools in 2001 by Sven Neumann  <sven@gimp.org>.
 * Included into blib in 2002 by Sven Neumann  <sven@gimp.org>.
 *
 * GIMP plug-in written by Adam D. Moss  <adam@gimp.org> <adam@foxbox.org>
 *
 * Based around original GIF code by David Koblas.
 *
 * This filter uses code taken from the "giftopnm" and "ppmtogif" programs
 *    which are part of the "netpbm" package.
 *
 *  "The Graphics Interchange Format(c) is the Copyright property of
 *  CompuServe Incorporated.  GIF(sm) is a Service Mark property of
 *  CompuServe Incorporated."
 */

/* Copyright notice for GIF code from which this plugin was long ago     *
 * derived (David Koblas has granted permission to relicense):           *
 * +-------------------------------------------------------------------+ *
 * | Copyright 1990, 1991, 1993, David Koblas.  (koblas@extra.com)     | *
 * +-------------------------------------------------------------------+ */

#include <gif/GifIO.h>

#define MAX_LZW_BITS       12
#define INTERLACE          0x40
#define LOCALCOLORMAP      0x80
#define BitSet(byte, bit)  (((byte) & (bit)) == (bit))
#define LM_to_uint(a,b)    (((b)<<8)|(a))

static int ReadColorMap (FILE    *io, int number, unsigned char *buffer);
static int GetDataBlock (FILE    *io, unsigned char *buf);
static int GetCode      (FILE    *io, int code_size, int flag);
static int LZWReadByte  (FILE    *io, int flag, int input_code_size);
static int GIFNextPixel (void);

/*
 * a code_int must be able to hold 2**BITS values of type int, and also -1
 */

typedef int      code_int;
typedef long int count_int;

unsigned char *pixels;

static void Putword    (int, FILE *);
static void compress   (int, FILE *);
static void output     (code_int);
static void cl_block   (void);
static void cl_hash    (count_int);
static void writeerr   (void);
static void char_init  (void);
static void char_out   (int);
static void flush_char (void);

static long CountDown;

/*
 * Return the next pixel from the image
 */
int GIFNextPixel (void) {
	if (CountDown == 0) return EOF;
	--CountDown;

	return *pixels++;
}

/* public */

void GIFEncodeHeader (FILE *fp, int gif89, int Width, int Height, int Background, int BitsPerPixel, unsigned char *cmap) {
	int B;
	int Resolution;
	int ColorMapSize;
	int i;

	ColorMapSize = 1 << BitsPerPixel;
	Resolution = BitsPerPixel;
	
	/* Write the Magic header */
	fwrite (gif89 ? "GIF89a" : "GIF87a", 1, 6, fp);

	/* Write out the screen width and height */
	Putword (Width, fp);
	Putword (Height, fp);

	/* Indicate that there is a global colour map */
	B = 0x80;	/* Yes, there is a color map */

	/* OR in the resolution */
	B |= (Resolution - 1) << 5;

	/* OR in the Bits per Pixel */
	B |= (BitsPerPixel - 1);

	/* Write it out */
	fputc (B, fp);

	/* Write out the Background colour */
	fputc (Background, fp);

	/* Byte of 0's (future expansion) */
	fputc (0, fp);

	/* Write out the Global Colour Map */
	for (i = 0; i < 3 * ColorMapSize; i++)	fputc (cmap[i], fp);
}

void GIFEncodeGraphicControlExt (FILE *fp, GIFDisposeType  Disposal, int Delay, int Animation, int Transparent) {

	Delay /= 10;   /* the external API is ms */

	/* Write out extension for transparent colour index, if necessary. */
	if ( (Transparent >= 0) || (Animation) ) {
		/* Extension Introducer - fixed. */
		fputc ('!', fp);
		/* Graphic Control Label - fixed. */
		fputc (0xf9, fp);
		/* Block Size - fixed. */
		fputc (4, fp);
		/* Packed Fields - XXXdddut (d=disposal, u=userInput, t=transFlag) */
		/*                    s8421                                        */
		fputc ( ((Transparent >= 0) ? 0x01 : 0x00) | (Animation ? (Disposal << 2) : 0x00),  fp);
        /* TRANSPARENCY */
		/* DISPOSAL */
	      
		/* 0x03 or 0x01 build frames cumulatively */
		/* 0x02 clears frame before drawing */
		/* 0x00 'don't care' */
		fputc (Delay & 0xFF, fp);
		fputc ((Delay >> 8) & 0xFF, fp);

		fputc (Transparent, fp);
		fputc (0, fp);
	}
}

void GIFEncodeImageData (FILE *fp, int Width, int Height, int BitsPerPixel, int offset_x, int offset_y, unsigned char *data) {
	int Resolution;
	int ColorMapSize;
	int InitCodeSize;

	ColorMapSize = 1 << BitsPerPixel;
	Resolution = BitsPerPixel;

	/* Calculate number of bits we are expecting */
	CountDown = (long) Width * (long) Height;
	pixels = data;

	/* The initial code size */
	if (BitsPerPixel <= 1)
		InitCodeSize = 2;
	else
		InitCodeSize = BitsPerPixel;

	/* Write an Image separator */
	fputc (',', fp);

	/* Write the Image header */
	Putword (offset_x, fp);
	Putword (offset_y, fp);
	Putword (Width, fp);
	Putword (Height, fp);

	/* no interlacing */
	fputc (0x0, fp);

	/* Write out the initial code size */
	fputc (InitCodeSize, fp);

	/* Go and actually compress the data */
	compress (InitCodeSize + 1, fp);

	/* Write out a Zero-length packet (to end the series) */
	fputc (0x0, fp);
}

void GIFEncodeClose (FILE *fp) {
	/* Write the GIF file terminator */
	fputc (';', fp);

	/* And close the file */
	fclose (fp);
}

void GIFEncodeLoopExt (FILE *fp, int num_loops) {
	fputc(0x21,fp);
	fputc(0xff,fp);
	fputc(0x0b,fp);
	fputs("NETSCAPE2.0",fp);
	fputc(0x03,fp);
	fputc(0x01,fp);
	Putword(num_loops,fp);
	fputc(0x00,fp);

	/* NOTE: num_loops==0 means 'loop infinitely' */
}

void GIFEncodeCommentExt (FILE *fp, char *comment) {
	if (!comment || !*comment) return;

	if (strlen(comment)>240) {
		fprintf (stderr,"GIF: warning: comment too large - comment block not written.\n");
		return;	
	}

	fputc(0x21,fp);
	fputc(0xfe,fp);
	fputc((int)strlen(comment),fp);
	fputs((const char *)comment,fp);
	fputc(0x00,fp);
}

/* Write out a word to the GIF file */
static void Putword (int w, FILE *fp) {
	fputc (w & 0xff, fp);
	fputc ((w / 256) & 0xff, fp);
}


/***************************************************************************
 *
 *  GIFCOMPR.C       - GIF Image compression routines
 *
 *  Lempel-Ziv compression based on 'compress'.  GIF modifications by
 *  David Rowley (mgardi@watdcsu.waterloo.edu)
 *
 ***************************************************************************/

/* General DEFINEs */

#define GIF_BITS    12
#define HSIZE  5003		/* 80% occupancy */

#ifdef NO_UCHAR
typedef char char_type;
#else /*NO_UCHAR */
typedef unsigned char char_type;
#endif /*NO_UCHAR */

/*
 * GIF Image compression - modified 'compress'
 *
 * Based on: compress.c - File compression ala IEEE Computer, June 1984.
 *
 * By Authors:  Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *              Jim McKie               (decvax!mcvax!jim)
 *              Steve Davies            (decvax!vax135!petsd!peora!srd)
 *              Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *              James A. Woods          (decvax!ihnp4!ames!jaw)
 *              Joe Orost               (decvax!vax135!petsd!joe)
 *
 */
#include <ctype.h>
#define ARGVAL() (*++(*argv) || (--argc && *++argv))

static int n_bits;		        /* number of bits/code */
static int maxbits = GIF_BITS;	/* user settable max # bits/code */
static code_int maxcode;	    /* maximum code, given n_bits */
static code_int maxmaxcode = (code_int) 1 << GIF_BITS;	/* should NEVER generate this code */
#ifdef COMPATIBLE		        /* But wrong! */
#define MAXCODE(Mn_bits)        ((code_int) 1 << (Mn_bits) - 1)
#else /*COMPATIBLE */
#define MAXCODE(Mn_bits)        (((code_int) 1 << (Mn_bits)) - 1)
#endif /*COMPATIBLE */

static count_int htab[HSIZE];
static unsigned short codetab[HSIZE];
#define HashTabOf(i)       htab[i]
#define CodeTabOf(i)    codetab[i]

const code_int hsize = HSIZE;	

/* the original reason for this being
 * variable was "for dynamic table sizing",
 * but since it was never actually changed
 * I made it const   --Adam. 
 */

/*
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**GIF_BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */

#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i) ((char_type*)(htab))[i]
#define de_stack        ((char_type*)&tab_suffixof((code_int)1<<GIF_BITS))

static code_int free_ent = 0;	/* first unused entry */

/*
 * block compression parameters -- after all codes are used up,
 * and compression rate changes, start over.
 */
static int clear_flg = 0;

static int offset;
static long int in_count = 1;	/* length of input */
static long int out_count = 0;	/* # of codes output (for debugging) */

/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */

static int				g_init_bits;
static FILE				*g_outfile;
static int				ClearCode;
static int				EOFCode;
static unsigned long	cur_accum;
static int				cur_bits;

static unsigned long masks[] =
{0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
 0x001F, 0x003F, 0x007F, 0x00FF,
 0x01FF, 0x03FF, 0x07FF, 0x0FFF,
 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};


static void compress (int init_bits, FILE *outfile) {
	register long fcode;
	register code_int i /* = 0 */ ;
	register int c;
	register code_int ent;
	register code_int disp;
	register code_int hsize_reg;
	register int hshift;

	/*
	 * Set up the globals:  g_init_bits - initial number of bits
	 *                      g_outfile   - pointer to output file
	 */
	g_init_bits = init_bits;
	g_outfile = outfile;

	cur_bits = 0;
	cur_accum = 0;

	/* Set up the necessary values */
	offset = 0;
	out_count = 0;
	clear_flg = 0;
	in_count = 1;

	ClearCode = (1 << (init_bits - 1));
	EOFCode = ClearCode + 1;
	free_ent = ClearCode + 2;

	/* Had some problems here... should be okay now.  --Adam */
	n_bits = g_init_bits;
	maxcode = MAXCODE (n_bits);

	char_init ();

	ent = GIFNextPixel ();

	hshift = 0;
	for (fcode = (long) hsize; fcode < 65536L; fcode *= 2L) ++hshift;
	hshift = 8 - hshift;		/* set hash code range bound */

	hsize_reg = hsize;
	cl_hash ((count_int) hsize_reg);	/* clear hash table */

	output ((code_int) ClearCode);

	while ((c = GIFNextPixel()) != EOF) {
		++in_count;

		fcode = (long) (((long) c << maxbits) + ent);
		i = (((code_int) c << hshift) ^ ent);	/* xor hashing */

		if (HashTabOf (i) == fcode) {
			ent = CodeTabOf (i);
			continue;
		}
		else if ((long) HashTabOf (i) < 0)	/* empty slot */
		goto nomatch;
		disp = hsize_reg - i;	/* secondary hash (after G. Knott) */
		if (i == 0) disp = 1;
probe:
		if ((i -= disp) < 0) i += hsize_reg;
		if (HashTabOf (i) == fcode) {
			ent = CodeTabOf (i);
			continue;
		}
		if ((long) HashTabOf (i) > 0) goto probe;

nomatch:
		output ((code_int) ent);
		++out_count;
		ent = c;

		if (free_ent < maxmaxcode) {
			CodeTabOf (i) = free_ent++;	/* code -> hashtable */
			HashTabOf (i) = fcode;
		}
		else
			cl_block ();
	}
	/* Put out the final code. */
	output ((code_int) ent);
	++out_count;
	output ((code_int) EOFCode);
}

/*****************************************************************
 * TAG( output )
 *
 * Output the given code.
 * Inputs:
 *      code:   A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *              that n_bits =< (long)wordsize - 1.
 * Outputs:
 *      Outputs code to the file.
 * Assumptions:
 *      Chars are 8 bits long.
 * Algorithm:
 *      Maintain a GIF_BITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */

static void output (code_int code) {
	cur_accum &= masks[cur_bits];

	if (cur_bits > 0)
		cur_accum |= ((long) code << cur_bits);
	else
		cur_accum = code;

	cur_bits += n_bits;

	while (cur_bits >= 8) {
		char_out ((unsigned int) (cur_accum & 0xff));
		cur_accum >>= 8;
		cur_bits -= 8;
	}

	/*
	 * If the next entry is going to be too big for the code size,
	 * then increase it, if possible.
	 */
	if (free_ent > maxcode || clear_flg) {
		if (clear_flg) {
			maxcode = MAXCODE (n_bits = g_init_bits);
			clear_flg = 0;
		}
		else {
			++n_bits;
			if (n_bits == maxbits)
				maxcode = maxmaxcode;
			else
				maxcode = MAXCODE (n_bits);
		}
	}

	if (code == EOFCode) {
		/* At EOF, write the rest of the buffer. */
		while (cur_bits > 0) {
			char_out ((unsigned int) (cur_accum & 0xff));
			cur_accum >>= 8;
			cur_bits -= 8;
		}
		flush_char ();
		fflush (g_outfile);

		if (ferror (g_outfile)) writeerr ();
	}
}

/* Clear out the hash table */
static void cl_block () {			/* table clear for block compress */
	cl_hash ((count_int) hsize);
	free_ent = ClearCode + 2;
	clear_flg = 1;

	output ((code_int) ClearCode);
}

static void cl_hash (register count_int hsize) {			/* reset code table */
	register count_int *htab_p = htab + hsize;
	register long i;
	register long m1 = -1;

	i = hsize - 16;
	do {				/* might use Sys V memset(3) here */
		*(htab_p - 16) = m1;
		*(htab_p - 15) = m1;
		*(htab_p - 14) = m1;
		*(htab_p - 13) = m1;
		*(htab_p - 12) = m1;
		*(htab_p - 11) = m1;
		*(htab_p - 10) = m1;
		*(htab_p - 9) = m1;
		*(htab_p - 8) = m1;
		*(htab_p - 7) = m1;
		*(htab_p - 6) = m1;
		*(htab_p - 5) = m1;
		*(htab_p - 4) = m1;
		*(htab_p - 3) = m1;
		*(htab_p - 2) = m1;
		*(htab_p - 1) = m1;
		htab_p -= 16;
	}
	while ((i -= 16) >= 0);

	for (i += 16; i > 0; --i) *--htab_p = m1;
}

static void writeerr () {
  fprintf (stderr, "GIF: error writing output file\n");
  return;
}

/******************************************************************************
 *
 * GIF Specific routines
 *
 ******************************************************************************/

/* Number of characters so far in this 'packet' */
static int a_count;

/* Set up the 'byte output' routine */
static void char_init () {
  a_count = 0;
}

/* Define the storage for the packet accumulator */
static char accum[256];

/*
 * Add a character to the end of the current packet, and if it is 254
 * characters, flush the packet to disk.
 */
static void char_out (int c) {
	accum[a_count++] = c;
	if (a_count >= 254)
		flush_char ();
}

/*
 * Flush the packet to disk, and reset the accumulator
 */
static void flush_char (void) {
	if (a_count > 0) {
		fputc (a_count, g_outfile);
		fwrite (accum, 1, a_count, g_outfile);
		a_count = 0;
	}
}

static int ReadOK (FILE *io, unsigned char *buffer, int len) { /*rewrited by Andrea Viarengo */
	int c;
	len++;

	while(--len) {
		c=fgetc(io);
		if(feof(io)) {
			//printf("End of File\n");		
			break;
		}
		*(buffer++)=(unsigned char)c;
	}
	return (len == 0);
}

int GIFDecodeHeader (FILE *io, int skip_magic, int *Width, int *Height, int *Background, int *colors, unsigned char **cmap) {
	int           AspectRatio;
	unsigned char buf[16];
	char          version[4];

	if (!skip_magic) {
		if (!ReadOK (io, buf, 6)) {
			printf("GIF: error reading magic number\n");
			return 0;
		}

		if (strncmp ((char *) buf, "GIF", 3) != 0) {
			printf("GIF: not a GIF file\n");
			return 0;
		}

		strncpy (version, (char *) buf + 3, 3);
		version[3] = '\0';

		if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0)) {
			printf("GIF: bad version number, not '87a' or '89a' ver=%s buf=%s\n",version,buf);
			return 0;
		}
	}

	//printf("after skip\n");

	if (!ReadOK (io, buf, 7)) {
		//printf("GIF: failed to read screen descriptor\n");
		return 0;
	}

	*Width  = LM_to_uint (buf[0], buf[1]);
	*Height = LM_to_uint (buf[2], buf[3]);
	*colors = 2 << (buf[4] & 0x07);
  
	if (Background) *Background = buf[5];

	AspectRatio = buf[6];
	if (AspectRatio != 0 && AspectRatio != 49) printf("GIF: warning - non-square pixels\n");

	//printf("GIF: w=%d h=%d c=%d\n",*Width,*Height,*colors);
	
	*cmap = NULL;

	if (BitSet (buf[4], LOCALCOLORMAP)) {
		unsigned char *colormap;
		
		//printf("GIF:read color map\n"); 

		colormap = (unsigned char *) calloc (3 * (*colors),sizeof(unsigned char) );

		/* Global Colormap */
		if (!ReadColorMap (io, *colors, colormap)) {
			printf("GIF: error reading global colormap\n");
			free (colormap);
			return 0;
		}
        //printf("GIF:read color map done!!\n"); 
		*cmap = colormap;
		//printf("GIF:read color map done 2!!\n"); 
    }
    //printf("GIF:return\n");
	return 1;
}

int	GIFDecodeRecordType (FILE *io, GIFRecordType  *type) {
	int c;

	//printf("GIF:start decode Record type\n");

	c=fgetc(io);
	if (c==EOF) {
		printf("GIF: EOF / read error on image data\n");
		return 0;
	}

	//printf("c=%x\n",c);

	switch ((unsigned char)c) {
		case ',':	*type = IMAGE; return 1;
		case '!':   //printf("!\n");
					c=fgetc(io);
					//printf("%x\n",c);
					if (c==EOF) {
						printf("GIF: EOF / read error on extension function code\n");
						return 0;
					}
					switch ((unsigned char)c) {
						case 0xf9:	*type = GRAPHIC_CONTROL_EXTENSION; break;
						case 0xfe:	*type = COMMENT_EXTENSION; break;
						case 0x01:	/* PLAINTEXT_EXTENSION   */
						case 0xff:	/* APPLICATION_EXTENSION */
									*type = UNKNOWN_EXTENSION; break;
						default:	return 0;
					}
					return 1;

		case ';':	*type = TERMINATOR; return 1;
		default:	//printf("GIF: bogus character 0x%02x, ignoring\n", c);
					return GIFDecodeRecordType (io, type);
    }
	//printf("GIF:type=%d\n",*type); 
	return 0;
}

int	GIFDecodeImage (FILE *io, int *Width, int *Height, int *offx, int *offy, int *colors, unsigned char **cmap, unsigned char  *data) {
	unsigned char  buf[16];
	int c;
	unsigned char *dest;
	int local_cmap;
	int interlace;
	int v;
	int width, height;
	int xpos = 0, ypos = 0, pass = 0;

	if (!ReadOK (io, buf, 9)) {
		printf("GIF: couldn't read image data\n");
		return 0;
    }

	*colors = 0;
	*cmap   = NULL;

	local_cmap = BitSet (buf[8], LOCALCOLORMAP);

	if (local_cmap) {
		unsigned char *colormap;
		*colors = 2 << (buf[8] & 0x07);
		
		colormap = (unsigned char *) calloc (3 * (*colors),sizeof(unsigned char) );

		if (!ReadColorMap (io, *colors, colormap)) {
			free (colormap);
			printf("GIF: error reading local colormap\n");
			return 0;
		}
		*cmap = colormap;
    }

	*Width  = width  = LM_to_uint (buf[4], buf[5]);
	*Height = height = LM_to_uint (buf[6], buf[7]);
	*offx   = LM_to_uint (buf[0], buf[1]);
	*offy   = LM_to_uint (buf[2], buf[3]);

	interlace = BitSet (buf[8], INTERLACE);

	c=fgetc(io);
	if (c==EOF) {
		printf("GIF: EOF / read error on image data\n");
		goto error;
    }

	if (LZWReadByte (io, 1, c) < 0) {
		printf("GIF: error while reading\n");
		goto error;
    }

	while ((v = LZWReadByte (io, 0, c)) >= 0) {
		dest = data + (ypos * width) + xpos;
		*dest = (unsigned char) v;

		xpos++;
		if (xpos == width) {
			xpos = 0;
			if (interlace) {
				switch (pass) {
					case 0:
					case 1: ypos += 8; break;
					case 2: ypos += 4; break;
					case 3: ypos += 2; break;
				}
				if (ypos >= height) {
					pass++;
					switch (pass) {
						case 1: ypos = 4; break;
						case 2: ypos = 2; break;
						case 3:	ypos = 1; break;
						default: goto fini;
					}
				}
			} else {ypos++;} /*if (interlace) */
		}
		if (ypos >= height) break;
	} /* while */

fini:
	if (LZWReadByte (io, 0, c) >= 0)
	printf("GIF: too much input data, ignoring extra...\n");
	return 1;

error:
	free (*cmap);
	*cmap = NULL;
	return 0;
}

int GIFDecodeGraphicControlExt (FILE *io, GIFDisposeType *Disposal, int *Delay, int *Transparent) {
	unsigned char buf[256];

	if (GetDataBlock (io, buf) < 4) return 0;
	
	*Disposal = (GIFDisposeType)((buf[0] >> 2) & 0x7);
	*Delay    = LM_to_uint (buf[1], buf[2]) * 10;  /* the external API is ms */

	if ((buf[0] & 0x1) != 0)
		*Transparent = buf[3];
	else
		*Transparent = -1;

	while (GetDataBlock (io, buf) > 0);

	return 1;
}

int GIFDecodeCommentExt0 (FILE  *io, char **comment) {
	char buf[256];
	int  comment_len;
	int  len;

	//printf("Comment\n");

	*comment = NULL;
	comment_len = 0;

	while ((len = GetDataBlock (io, (unsigned char *) buf)) > 0) {
		*comment = (char *) realloc (*comment, comment_len + len + 1);
		strncpy (*comment + comment_len, buf, len);
		comment_len += len;
    }
	//printf("Comment=%s\n",*comment);
	return 1;
}
int GIFDecodeCommentExt (FILE  *io, char **comment) {
	int  len;

	//printf("Comment\n");

	len=fgetc(io);
	printf("len=%d\n",len);

	*comment = (char *)malloc(len+1);
    
	fgets(*comment,len+1,io);
    
	len=fgetc(io); /*00*/
	//printf("Comment=%s (%d)\n",*comment,len);
	return 1;
}

void GIFDecodeUnknownExt (FILE  *io) {
  unsigned char buf[256];

  while (GetDataBlock (io, buf) > 0);
}

static int ReadColorMap (FILE    *io, int number, unsigned char *cmap) {
	int i;

    //printf("GIF: Readcolormap()\n");
	for (i = 0; i < 3 * number; i++) cmap[i]=fgetc (io);

    //printf("GIF: Readcolormap() end\n");
	return 1;
}

int ZeroDataBlock = 0;

static int	GetDataBlock (FILE *io, unsigned char *buf) {
	int count;
	
	count=fgetc(io);
	if (count==EOF) {
		printf("GIF: error in getting DataBlock size\n");
		return -1;
	}
	ZeroDataBlock = count == 0;

	if ((count != 0) && (!ReadOK (io, buf, count))) {
		printf("GIF: error in reading DataBlock\n");
		return -1;
    }
	return count;
}

static int GetCode (FILE *io, int code_size, int flag) {
	static unsigned char buf[280];
	static int curbit, lastbit, done, last_byte;
	int i, j, ret;
	unsigned char count;

	if (flag) {
		curbit = 0;
		lastbit = 0;
		done = 0;
		last_byte = 2;
		return 0;
	}
	
	if ((curbit + code_size) >= lastbit) {
		if (done) {
			if (curbit >= lastbit)
				printf("GIF: ran off the end of my bits\n");
			return -1;
		}

		buf[0] = buf[last_byte - 2];
		buf[1] = buf[last_byte - 1];

		if ((count = GetDataBlock (io, &buf[2])) <= 0) done = 1;

		last_byte = 2 + count;
		curbit = (curbit - lastbit) + 16;
		lastbit = (2 + count) * 8;
	}

	ret = 0;
	for (i = curbit, j = 0; j < code_size; ++i, ++j) ret |= ((buf[i / 8] & (1 << (i % 8))) != 0) << j;
	curbit += code_size;
	return ret;
}

static int LZWReadByte (FILE *io, int flag, int input_code_size) {
	static int fresh = 0;
	int code, incode;
	static int code_size, set_code_size;
	static int max_code, max_code_size;
	static int firstcode, oldcode;
	static int clear_code, end_code;
	static int table[2][(1 << MAX_LZW_BITS)];
	static int stack[(1 << (MAX_LZW_BITS)) * 2], *sp;
	register int i;
	
	if (flag) {
		set_code_size = input_code_size;
		code_size = set_code_size + 1;
		clear_code = 1 << set_code_size;
		end_code = clear_code + 1;
		max_code_size = 2 * clear_code;
		max_code = clear_code + 2;
		
		GetCode (io, 0, 1);

		fresh = 1;
		
		for (i = 0; i < clear_code; ++i) {
			table[0][i] = 0;
			table[1][i] = i;
		}
		for (; i < (1 << MAX_LZW_BITS); ++i) table[0][i] = table[1][0] = 0;
		
		sp = stack;
		return 0;
	}
	else if (fresh) {
		fresh = 0;
		do {
			firstcode = oldcode = GetCode (io, code_size, 0);
		}
		while (firstcode == clear_code);
		return firstcode;
	}
	
	if (sp > stack) return *--sp;
	
	while ((code = GetCode (io, code_size, 0)) >= 0) {
		if (code == clear_code) {
			for (i = 0; i < clear_code; ++i) {
				table[0][i] = 0;
				table[1][i] = i;
			}
			for (; i < (1 << MAX_LZW_BITS); ++i) table[0][i] = table[1][i] = 0;
			
			code_size = set_code_size + 1;
			max_code_size = 2 * clear_code;
			max_code = clear_code + 2;
			sp = stack;
			firstcode = oldcode = GetCode (io, code_size, 0);
			return firstcode;
		}
		else if (code == end_code) {
			int count;
			unsigned char buf[260];
			
			if (ZeroDataBlock) return -2;
			while ((count = GetDataBlock (io, buf)) > 0);
			
			if (count != 0) printf("GIF: missing EOD in data stream (common occurence)");
			return -2;
		}

		incode = code;

		if (code >= max_code) {
			*sp++ = firstcode;
			code = oldcode;
		}
		
		while (code >= clear_code) {
			*sp++ = table[1][code];
			if (code == table[0][code]) {
				printf("GIF: circular table entry BIG ERROR\n");
				return -2;
			}
			code = table[0][code];
		}

		*sp++ = firstcode = table[1][code];

		if ((code = max_code) < (1 << MAX_LZW_BITS)) {
			table[0][code] = oldcode;
			table[1][code] = firstcode;
			++max_code;
			if ((max_code >= max_code_size) && (max_code_size < (1 << MAX_LZW_BITS))) {
				max_code_size *= 2;
				++code_size;
			}
		}
		
		oldcode = incode;

		if (sp > stack) return *--sp;
	}
	return code;
}
