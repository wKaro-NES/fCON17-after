import struct

def read_bmp_file(file_path):
    with open(file_path, 'rb') as f:
        bmp_header = f.read(14)  # BITMAPFILEHEADER
        header_fields = struct.unpack('<2sIHHI', bmp_header)

        if header_fields[0] != b'BM':
            raise ValueError("Not a BMP file")

        dib_header = f.read(40)  # assume BITMAPINFOHEADER
        dib_fields = struct.unpack('<IIIHHIIIIII', dib_header)

        dib_header_len = dib_fields[0]
        if dib_header_len != 40: # validate BITMAPINFOHEADER == 40
            raise NotImplementedError("BITMAPINFOHEADER expected (Win NT, 3.1x or later)")

        width = dib_fields[1]
        height = dib_fields[2]
        bit_count = dib_fields[4]
        data_offset = header_fields[4]

        print(f"BMP info: {width}x{height}, {bit_count} bpp")

        #FIXME validate color count
        #FIXME print color table
        if width != 256 or height >240:
            raise NotImplementedError("BMP should be 256 pixel wide and less or equal 240 pixel height.")

        if bit_count != 4:
            raise NotImplementedError("Only 4-bit BMPs (and 4 colors) are supported.")

        f.seek(data_offset)

        # BMP rows are padded to 4-byte boundaries
        row_size = ((bit_count * width + 31) // 32) * 4
        pixel_array = []

        for _ in range(height):
            row_data = f.read(row_size)
            row = []
            for byte in range(row_size):
                row.append((row_data[byte] & 0xF0) >> 4)    #1st nibble
                row.append(row_data[byte] & 0x0F)           #2nd nibble
            pixel_array.append(row)
        return width, height, pixel_array[::-1]  # flip vertically

def chunky_to_bitplanes(pixel_array, bit_depth=4):
    h = len(pixel_array)
    w = len(pixel_array[0])
    bp = [[0] * (w * h) for _ in range(bit_depth)]

    for y in range(h):
        for x in range(w):
            pixel = pixel_array[y][x]
            for b in range(bit_depth):
                bit = (pixel >> b) & 1
                bp[b][y * w + x] = bit

    return bp

def print_bitplanes(bp, w, h):
    for b, plane in enumerate(bp):
        print(f"\nBitplane {b}:")
        for y in range(h):
            row = plane[y * w:(y + 1) * w]
            print(''.join(str(bit) for bit in row))

def convert_to_byte(bits):
    byte = 0
    for i in range(0,8):
        byte += bits[i] << (7-i)
    return str.format('${0:02x}', byte)

def print_chr(bp, w, h):
    for y in range(0, h, 8):
        for x in range(0, w, 8):
            chr1 = chr2 = ''
            for line in range(0,8):
                if y + line < h:
                    bit_index = x + (y + line) * w
                    chr1 += convert_to_byte(bp[0][bit_index: bit_index + 8]) + ','
                    chr2 += convert_to_byte(bp[1][bit_index: bit_index + 8])
                else:
                    chr1 += '0,'
                    chr2 += '0'
                if line < 7:
                    chr2 += ','
            print('.byte ' + chr1 + chr2)

# run
if __name__ == "__main__":

    #FIXME - args && usage
    #bmp_file = "rodzinne-1-white-star.bmp"
    bmp_file = "rodzinne-2-black-star.bmp"

    width, height, pixels = read_bmp_file(bmp_file)

    #convert chunky(array of bytes per pixel) to bitplanes.
    bitplanes = chunky_to_bitplanes(pixels, bit_depth=4)

    #show bits
    print_bitplanes(bitplanes, width, height)

    #convert to NES pattern
    print_chr(bitplanes, width, height)