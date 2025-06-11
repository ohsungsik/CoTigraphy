#include "pch.hpp"
#include "WebPWriter.hpp"

#include <webp/encode.h>
#include <webp/mux.h>

namespace CoTigraphy
{
	WebPWriter::WebPWriter() noexcept
		= default;

	WebPWriter::~WebPWriter()
		= default;

	void WebPWriter::SaveToFile(const std::wstring& fileName, uint8_t* buffer, const size_t& bufferSize,
		const int width, const int height)
	{
		UNREFERENCED_PARAMETER(bufferSize);

		// 초기화
		WebPAnimEncoderOptions enc_options;
		WebPAnimEncoderOptionsInit(&enc_options);

		WebPAnimEncoder* const encoder = WebPAnimEncoderNew(width, height, &enc_options);
		ASSERT(encoder);

		WebPPicture pic;
		WebPPictureInit(&pic);
		pic.width = width;
		pic.height = height;
		pic.use_argb = 1;

		WebPConfig config;
		WebPConfigInit(&config);
		config.quality = 90.0f;

		// RGBA → WebPPicture 로 변환
		int ret = WebPPictureImportRGBA(&pic, buffer, width * 4);
		ASSERT(ret != 0);

		// 프레임 추가
		ret = WebPAnimEncoderAdd(encoder, &pic, 0, &config);
		ASSERT(ret != 0);

		// 마지막 frame 마킹
		ret = WebPAnimEncoderAdd(encoder, nullptr, 0, nullptr);
		ASSERT(ret != 0);

		// WebP 애니메이션 출력
		WebPData webpData;
		WebPDataInit(&webpData);
		ret = WebPAnimEncoderAssemble(encoder, &webpData);
		ASSERT(ret != 0);

		// 파일로 저장
		FILE* file;
		_wfopen_s(&file, fileName.c_str(), L"wb");
		ASSERT(file != nullptr);

		std::ignore = fwrite(webpData.bytes, webpData.size, 1, file);
		std::ignore = fclose(file);

		WebPDataClear(&webpData);
		WebPAnimEncoderDelete(encoder);
		WebPPictureFree(&pic);
	}
}
