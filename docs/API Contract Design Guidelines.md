# Method Contract Design Guidelines for New Developers

## Table of Contents

1. [Contract Pattern Overview](#contract-pattern-overview)
2. [Decision Framework](#decision-framework)
3. [Implementation Guidelines](#implementation-guidelines)
4. [Code Examples](#code-examples)
5. [Best Practices](#best-practices)
6. [Common Pitfalls](#common-pitfalls)

---

## Contract Pattern Overview

### Strong Contract Pattern

**Definition**: The caller must guarantee all preconditions are met before invoking the method.

**Characteristics**:
- Uses `assert()` or `ASSERT()` macros for precondition validation
- Treats invalid input as programmer error
- No runtime error handling for invalid parameters
- Optimized for performance-critical paths

**When to Use**:
- Private/internal methods
- Performance-critical code paths
- Internal algorithm invariants
- Clear usage patterns with well-defined preconditions

### Weak Contract Pattern

**Definition**: The method handles invalid input gracefully through runtime error checking.

**Characteristics**:
- Returns error codes or throws exceptions for invalid input
- Defensive programming approach
- Comprehensive input validation
- Suitable for external interfaces

**When to Use**:
- Public APIs exposed to external clients
- User input processing (CLI, GUI, network)
- File/disk I/O operations
- External library interfaces

### Hybrid Contract Pattern

**Definition**: Combines both approaches - provides runtime error handling while asserting in debug builds.

**Characteristics**:
- Returns error codes like Weak Contract
- Uses conditional assertions for debug builds
- Allows unit testing by disabling assertions
- Balances safety and development feedback

**When to Use**:
- Public APIs requiring both safety and developer feedback
- Internal APIs that need unit testing
- Methods handling mixed internal/external input

---

## Decision Framework

Use this decision tree to determine the appropriate contract pattern:

```
Is this method private/internal only?
├─ YES → Consider Strong Contract
│   └─ Is performance critical?
│       ├─ YES → Use Strong Contract
│       └─ NO → Consider Strong Contract (default)
│
└─ NO → Method is public/external
    └─ Does input come from external sources?
        ├─ YES → Use Weak Contract
        └─ NO → Is unit testing required?
            ├─ YES → Use Hybrid Contract
            └─ NO → Consider Strong Contract
```

### Quick Reference Checklist

| Criteria | Strong | Weak | Hybrid |
|----------|--------|------|--------|
| Internal use only | ✓ | ✗ | ✗ |
| External input possible | ✗ | ✓ | ✓ |
| Caller can guarantee preconditions | ✓ | ✗ | ✗ |
| Must maintain execution flow on error | ✗ | ✓ | ✓ |
| Unit testing required | ✗ | ✓ | ✓ |
| Performance critical | ✓ | ✗ | △ |

---

## Implementation Guidelines

### Strong Contract Implementation

```cpp
// Strong Contract: caller must guarantee preconditions
_Success_(return != nullptr)
_Ret_maybenull_
RenderTarget* GetRenderTarget(_In_ const uint32_t index) const noexcept
{
    // Precondition assertions
    assert(index < mMaxRenderTargets);
    assert(mRenderTargets != nullptr);
    
    return mRenderTargets[index];
}
```

**Key Points**:
- Use `assert()` for precondition validation
- Mark methods as `noexcept` when appropriate
- Include SAL annotations for all parameters
- Document preconditions clearly in comments

### Weak Contract Implementation

```cpp
// Weak Contract: handles invalid input gracefully
_Success_(return == S_OK)
HRESULT LoadTexture(
    _In_z_ const wchar_t* pFilePath,
    _Outptr_result_maybenull_ ID3D11Texture2D** ppTexture) noexcept
{
    if (pFilePath == nullptr)
    {
        return E_INVALIDARG;
    }
    
    if (ppTexture == nullptr)
    {
        return E_POINTER;
    }
    
    *ppTexture = nullptr;
    
    // Attempt to load texture
    const HRESULT hr = D3DX11CreateTextureFromFileW(
        mDevice, pFilePath, nullptr, nullptr, 
        reinterpret_cast<ID3D11Resource**>(ppTexture), nullptr);
    
    if (FAILED(hr))
    {
        LogError(L"Failed to load texture: %s", pFilePath);
        return hr;
    }
    
    return S_OK;
}
```

**Key Points**:
- Validate all input parameters
- Return meaningful error codes
- Initialize output parameters to safe values
- Log errors for debugging purposes

### Hybrid Contract Implementation

```cpp
// Hybrid Contract: handles invalid input gracefully with debug assertions
_Success_(return == S_OK)
HRESULT AddRenderCommand(
    _In_ const RenderCommand& command) noexcept
{
    // Validate input
    if (!command.IsValid())
    {
#ifndef UNIT_TESTING
        // Assert in debug builds to catch programmer errors
        assert(false && "Invalid render command provided");
#endif
        return E_INVALIDARG;
    }
    
    if (mCommandCount >= mMaxCommands)
    {
#ifndef UNIT_TESTING
        assert(false && "Command buffer overflow");
#endif
        return E_OUTOFMEMORY;
    }
    
    // Add command to buffer
    mCommands[mCommandCount++] = command;
    return S_OK;
}
```

**Key Points**:
- Use conditional compilation for assertions
- Provide runtime error handling
- Enable unit testing by disabling assertions
- Document the hybrid nature in comments

---

## Code Examples

### Example 1: Matrix Operations (Strong Contract)

```cpp
class Matrix4x4
{
private:
    // Strong Contract: internal helper method
    // Caller must ensure valid indices
    _Ret_notnull_
    float& GetElement(_In_ const uint32_t row, _In_ const uint32_t col) noexcept
    {
        assert(row < 4);
        assert(col < 4);
        return mData[row * 4 + col];
    }
    
public:
    // Weak Contract: public interface
    _Success_(return == S_OK)
    HRESULT SetElement(
        _In_ const uint32_t row, 
        _In_ const uint32_t col, 
        _In_ const float value) noexcept
    {
        if (row >= 4 || col >= 4)
        {
            return E_INVALIDARG;
        }
        
        GetElement(row, col) = value;
        return S_OK;
    }
};
```

### Example 2: Resource Manager (Hybrid Contract)

```cpp
class ResourceManager
{
public:
    // Hybrid Contract: public API with debug assertions
    _Success_(return == S_OK)
    HRESULT RegisterResource(
        _In_z_ const wchar_t* pName,
        _In_ IResource* pResource) noexcept
    {
        if (pName == nullptr)
        {
#ifndef UNIT_TESTING
            assert(false && "Resource name cannot be null");
#endif
            return E_INVALIDARG;
        }
        
        if (pResource == nullptr)
        {
#ifndef UNIT_TESTING
            assert(false && "Resource cannot be null");
#endif
            return E_POINTER;
        }
        
        if (wcslen(pName) == 0)
        {
#ifndef UNIT_TESTING
            assert(false && "Resource name cannot be empty");
#endif
            return E_INVALIDARG;
        }
        
        // Implementation...
        return S_OK;
    }
};
```

---

## Best Practices

### Documentation Standards

Always document contract type and expectations:

```cpp

// hpp file

#pragma once

#include "Matrix4x4.hpp"

/**
 * @brief Calculates the World-View-Projection (WVP) matrix.
 * @contract Strong - Caller must ensure all input matrices are valid.
 * @param worldMatrix World transformation matrix. Must not be null and must be properly initialized.
 * @param viewMatrix View matrix. Must not be null and must be properly initialized.
 * @param projMatrix Projection matrix. Must not be null and must be properly initialized.
 * @return Combined WVP matrix.
 * @pre worldMatrix, viewMatrix, projMatrix must be non-null.
 * @pre All matrices must be properly initialized.
 */
Matrix4x4 CalculateWVP(
    _In_ const Matrix4x4* worldMatrix,
    _In_ const Matrix4x4* viewMatrix,
    _In_ const Matrix4x4* projMatrix) noexcept;


// cpp file

/**
 * @brief Internal implementation of CalculateWVP.
 *
 * The WVP matrix is calculated in the following order:
 *     WVP = Projection * View * World
 *
 * - Matrix multiplication follows the convention of column-major order.
 * - The function does not perform validation on input matrices.
 * - The caller is responsible for ensuring that all input matrices are valid.
 * - This function is noexcept and will not throw exceptions.
 * 
 * Performance notes:
 * - Optimized for inline use in the main rendering loop.
 * - Minimal branching; assumes fast matrix multiply operator.
 */
Matrix4x4 CalculateWVP(
    const Matrix4x4* worldMatrix,
    const Matrix4x4* viewMatrix,
    const Matrix4x4* projMatrix) noexcept
{
    assert(worldMatrix != nullptr);
    assert(viewMatrix != nullptr);
    assert(projMatrix != nullptr);

    return (*projMatrix) * (*viewMatrix) * (*worldMatrix);
}

```

### Error Code Conventions

Use consistent error codes throughout the codebase:

```cpp
// Standard error codes for contract violations
constexpr HRESULT E_INVALID_CONTRACT_STATE = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1000);
constexpr HRESULT E_PRECONDITION_FAILED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1001);
constexpr HRESULT E_POSTCONDITION_FAILED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1002);
```

### Assertion Macros

Create consistent assertion macros for contract validation:

```cpp
#ifdef _DEBUG
    #define CONTRACT_ASSERT(condition, message) \
        assert((condition) && (message))
    #define CONTRACT_PRECONDITION(condition) \
        CONTRACT_ASSERT(condition, "Precondition failed")
    #define CONTRACT_POSTCONDITION(condition) \
        CONTRACT_ASSERT(condition, "Postcondition failed")
#else
    #define CONTRACT_ASSERT(condition, message) ((void)0)
    #define CONTRACT_PRECONDITION(condition) ((void)0)
    #define CONTRACT_POSTCONDITION(condition) ((void)0)
#endif
```

---

## Common Pitfalls

### 1. Inconsistent Contract Application

**Wrong**:
```cpp
class Renderer
{
    void SetupRenderState(int state) // Strong contract
    {
        assert(state >= 0);
        // ...
    }
    
    void SetRenderTarget(RenderTarget* target) // No contract!
    {
        mCurrentTarget = target; // Dangerous!
    }
};
```

**Right**:
```cpp
class Renderer
{
    void SetupRenderState(const int state) noexcept // Strong contract
    {
        CONTRACT_PRECONDITION(state >= 0);
        // ...
    }
    
    HRESULT SetRenderTarget(_In_opt_ RenderTarget* pTarget) noexcept // Weak contract
    {
        // Null is allowed - clear current target
        mCurrentTarget = pTarget;
        return S_OK;
    }
};
```

### 2. Missing SAL Annotations

**Wrong**:
```cpp
HRESULT CreateBuffer(void* data, size_t size, Buffer** buffer)
{
    // Missing annotations make contract unclear
}
```

**Right**:
```cpp
_Success_(return == S_OK)
HRESULT CreateBuffer(
    _In_reads_bytes_opt_(size) const void* pData,
    _In_ const size_t size,
    _Outptr_result_maybenull_ Buffer** ppBuffer) noexcept
{
    // Clear contract expectations
}
```

### 3. Mixing Contract Types Inappropriately

**Wrong**:
```cpp
HRESULT ProcessUserInput(const char* input) // Weak contract expected
{
    assert(input != nullptr); // Using Strong contract assertion!
    // This will crash on invalid user input
}
```

**Right**:
```cpp
_Success_(return == S_OK)
HRESULT ProcessUserInput(_In_opt_z_ const char* pInput) noexcept
{
    if (pInput == nullptr)
    {
        return E_INVALIDARG; // Proper Weak contract handling
    }
    // ...
}
```

---

## Conclusion

Method contracts are fundamental to creating maintainable, safe, and efficient C++ code. By following these guidelines:

1. **Start with Strong Contract** as the default choice
2. **Apply Weak Contract** for external interfaces and user input
3. **Use Hybrid Contract** when you need both safety and testability
4. **Document contract decisions** clearly in code and comments
5. **Be consistent** across your codebase

Remember: **Strong contracts optimize for performance and early error detection, while Weak contracts optimize for robustness and user experience. Choose the right tool for each situation.**

### Quick Reference Comments

Use these standardized comments in your code:

```cpp
// Strong Contract: caller must guarantee all preconditions
// Weak Contract: this method handles invalid input gracefully  
// Hybrid Contract: handles invalid input gracefully, asserts in debug builds
```

By applying these patterns consistently, you'll create code that is both robust and maintainable, meeting our safety-critical development standards while remaining efficient and testable.