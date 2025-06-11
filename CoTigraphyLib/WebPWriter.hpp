
#pragma once

namespace CoTigraphy
{
	class WebPWriter final
	{
	public:
		explicit WebPWriter() noexcept;

		WebPWriter(const WebPWriter& other) = delete;
		WebPWriter(WebPWriter&& other) = delete;

		WebPWriter& operator=(const WebPWriter& rhs) = delete;
		WebPWriter& operator=(WebPWriter&& rhs) = delete;

		~WebPWriter();

		void SaveToFile(const std::wstring& fileName, uint8_t* buffer, const size_t& bufferSize, const int width, const int height);
	};
}
