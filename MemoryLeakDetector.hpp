#pragma once

class MemoryLeakDetector final
{
public:
	explicit MemoryLeakDetector() noexcept;

	MemoryLeakDetector(const MemoryLeakDetector& other) = delete;
	MemoryLeakDetector(MemoryLeakDetector&& other) = delete;

	MemoryLeakDetector& operator=(const MemoryLeakDetector& rhs) = delete;
	MemoryLeakDetector& operator=(MemoryLeakDetector&& rhs) = delete;

	~MemoryLeakDetector();

	static void Initialize() noexcept;

private:
	static void OnProcessExit() noexcept;
};
