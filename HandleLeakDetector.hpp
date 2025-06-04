#pragma once

class HandleLeakDetector final
{
public:
	explicit HandleLeakDetector() noexcept;

	HandleLeakDetector(const HandleLeakDetector& other) = delete;
	HandleLeakDetector(HandleLeakDetector&& other) = delete;

	HandleLeakDetector& operator=(const HandleLeakDetector& rhs) = delete;
	HandleLeakDetector& operator=(HandleLeakDetector&& rhs) = delete;

	~HandleLeakDetector();

	static void Initialize() noexcept;

private:
	static void OnProcessExit() noexcept;
};

