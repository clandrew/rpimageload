
#include "pch.h"

// The target format is R5G6B5 with flipped endianness.
// So, it looks like
// 
// [0] - green 
// [1] - green 
// [2] - green (low)
// [3] - blue (high)
// [4] - blue
// [5] - blue
// [6] - blue 
// [7] - blue(low)
// [8] - red (high)
// [9] - red
// [10]- red
// [11]- red
// [12]- red(low)
// [13]- green (high)
// [14]- green 
// [15]- green 

ComPtr<IWICImagingFactory> EnsureWicImagingFactory()
{
    ComPtr<IWICImagingFactory> wicImagingFactory;

    CoInitialize(nullptr);

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory1,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory,
        (LPVOID*)&wicImagingFactory);

    return wicImagingFactory;
}


void PrintUsage()
{
    std::cout << "Usage: ColorConversion.exe SourceFile DestFile\n";
    std::cout << "    where SourceFile is a PNG image file, and\n";
    std::cout << "    where DestFile is a C++ header.\n";
    std::cout << "\n";
    std::cout << "For example, use\n";
    std::cout << "    ColorConversion.exe test_image_1.png output.data.h\n";
    std::cout << "and then output.data.h will have C++ literals for the image width, height, and data in byte-flipped R5G6B5 format.\n";
}


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        PrintUsage();
        return -1;
    }
        
    std::wstring inputFileName;
    std::string outputFileName;

    std::string shortPath;
    std::string shortName;
    {
        std::string sourceNarrow = argv[1];
        std::wstring wide(sourceNarrow.begin(), sourceNarrow.end());
        inputFileName = wide;

        size_t delim0 = sourceNarrow.rfind('\\');
        if (delim0 != std::string::npos && sourceNarrow.size() > 1)
        {
            delim0++;
        }
        else
        {
            delim0 = 0;
        }

        size_t delim1 = sourceNarrow.find('.', delim0);
        if (delim1 == std::string::npos) delim1 = sourceNarrow.size();

        shortName = sourceNarrow.substr(delim0, delim1 - delim0);
        shortPath = sourceNarrow.substr(delim0, sourceNarrow.size() - delim0);
    }
    
    outputFileName = argv[2];

    ComPtr<IWICImagingFactory> wicImagingFactory = EnsureWicImagingFactory();

    ComPtr<IWICBitmapDecoder> spDecoder;
    if (FAILED(wicImagingFactory->CreateDecoderFromFilename(
        inputFileName.c_str(),
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad, &spDecoder)))
    {
        return -1;
    }

    ComPtr<IWICBitmapFrameDecode> spSource;
    if (FAILED(spDecoder->GetFrame(0, &spSource)))
    {
        return -1;
    }

    // Convert the image format to 32bppPBGRA, equiv to DXGI_FORMAT_B8G8R8A8_UNORM
    // WIC doesn't support this kind of 16bit RGB format so we will manually convert.

    ComPtr<IWICFormatConverter> spConverter;
    if (FAILED(wicImagingFactory->CreateFormatConverter(&spConverter)))
    {
        return -1;
    }

    if (FAILED(spConverter->Initialize(
        spSource.Get(),
        GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut)))
    {
        return -1;
    }

    UINT retrievedWidth, retrievedHeight;

    if (FAILED(spConverter->GetSize(&retrievedWidth, &retrievedHeight)))
    {
        return -1;
    }

    if (retrievedWidth > INT_MAX)
    {
        return -1;
    }

    if (retrievedHeight > INT_MAX)
    {
        return -1;
    }

    std::vector<UINT> buffer32bppB8G8R8A8;
    buffer32bppB8G8R8A8.resize((size_t)retrievedWidth * retrievedHeight);
    assert(buffer32bppB8G8R8A8.size() < UINT_MAX);
    if (FAILED(spConverter->CopyPixels(
        nullptr, 
        retrievedWidth * sizeof(UINT),
        static_cast<UINT>(buffer32bppB8G8R8A8.size()) * sizeof(UINT),
        reinterpret_cast<BYTE*>(buffer32bppB8G8R8A8.data()))))
    {
        return -1;
    }

    std::vector<unsigned short> buffer16bppR5G6B5; // with flipped endianness because the platform expects it.

    for (size_t i = 0; i < buffer32bppB8G8R8A8.size(); ++i)
    {
        UINT sourcePx = buffer32bppB8G8R8A8[i];
        UINT b8 = sourcePx & 0xFF;
        sourcePx >>= 8;
        UINT g8 = sourcePx & 0xFF;
        sourcePx >>= 8;
        UINT r8 = sourcePx & 0xFF;
        
        UINT r5 = r8 / 8; // Scale [0..255] to [0..31]]
        UINT g6 = g8 / 4; // Scale [0..255] to [0..63]]
        UINT b5 = b8 / 8; // Scale [0..255] to [0..31]]

        UINT r5g6b5 = 0;
        r5g6b5 |= r5;
        r5g6b5 <<= 6;
        r5g6b5 |= g6;
        r5g6b5 <<= 5;
        r5g6b5 |= b5;

        UINT flip = r5g6b5;
        UINT destHigh = flip & 0xFF;
        flip >>= 8;
        UINT destLow = flip & 0xFF;

        unsigned short destPx = (destHigh << 8) | destLow;
        buffer16bppR5G6B5.push_back(destPx);
    }

    // Dump it all to a source file
    {
        std::ofstream out(outputFileName.c_str());
        out << "// Source: " << shortPath << "\n\n";
        out << "int " << shortName << "_width = " << retrievedWidth << "; \n";
        out << "int " << shortName << "_height = " << retrievedHeight << "; \n\n";
        out << "unsigned short " << shortName << "_data[] = {\n    ";
        for (size_t i = 0; i < buffer16bppR5G6B5.size(); ++i)
        {
            out << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << buffer16bppR5G6B5[i];
            if (i < buffer16bppR5G6B5.size() - 1)
            {
                out << ", ";
            }
            if (i % 16 == 15)
            {
                out << "\n    ";
            }
        }
        out << "\n};\n";
    }
}

