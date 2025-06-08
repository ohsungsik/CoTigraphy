# Method Documentation Standards

The goal is to help all developers (including new developers) write **consistent and easy-to-read comments** for both header (`.h`) and implementation (`.cpp`) files.

---

## Complete Header and Implementation Examples

### Strong Contract Example

**Header (`.h`):**
```cpp
/**
 * @brief Calculate World-View-Projection (WVP) matrix.
 * @contract Strong - Caller must ensure all input matrices are valid.
 * @param worldMatrix Pointer to World matrix. Must not be null.
 * @param viewMatrix Pointer to View matrix. Must not be null.
 * @param projMatrix Pointer to Projection matrix. Must not be null.
 * @return The combined WVP matrix.
 * @pre All matrices must be properly initialized and not null.
 */
Matrix4x4 CalculateWVP(
    _In_ const Matrix4x4* worldMatrix,
    _In_ const Matrix4x4* viewMatrix,
    _In_ const Matrix4x4* projMatrix) noexcept;
```

**Implementation (`.cpp`):**
```cpp
/**
 * @details
 * The World-View-Projection (WVP) matrix is calculated as:
 *     WVP = Projection * View * World
 *
 * Notes:
 * - Matrix multiplication uses column-major order.
 * - This function does not validate the input matrices (Strong contract).
 * - Performance optimized for per-frame rendering.
 * - No exception is thrown (noexcept).
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

### Weak Contract Example

**Header (`.h`):**
```cpp
/**
 * @brief Load an image from file with error handling.
 * @contract Weak - The method checks input parameters and handles errors.
 * @param filePath Path to the image file. Can be null or invalid.
 * @param imageOut Pointer to store loaded image. Must not be null.
 * @return S_OK if success, E_INVALIDARG for bad parameters, E_FAIL for file errors.
 * @pre imageOut must not be null.
 */
_Check_return_ HRESULT LoadImage(
    _In_opt_z_ const wchar_t* filePath,
    _Outptr_ Image** imageOut) noexcept;
```

**Implementation (`.cpp`):**
```cpp
/**
 * @details
 * This function uses an internal image loader to read the file.
 * Input validation:
 * - Checks if filePath is null or empty
 * - Validates file existence and accessibility
 * - Ensures imageOut is not null
 * 
 * Memory management:
 * - The image is allocated on success; the caller must release it.
 * - No memory is allocated on failure.
 * - Function is exception-safe (noexcept).
 */
HRESULT LoadImage(
    const wchar_t* filePath,
    Image** imageOut) noexcept
{
    if (imageOut == nullptr)
    {
        return E_INVALIDARG;
    }

    *imageOut = nullptr;

    if (filePath == nullptr || wcslen(filePath) == 0)
    {
        return E_INVALIDARG;
    }

    // File existence check
    if (!PathFileExists(filePath))
    {
        return E_FAIL;
    }

    // ... image loading logic ...

    return S_OK;
}
```

## Documentation Template Integration

### Complete Template for New Developers

**Header File (`.h`) Template:**
```cpp
/**
 * @brief [Short description of what this method does]
 * @contract [Strong / Weak / Hybrid] - [Explanation of contract]
 * @param param1 [Description of param1 and constraints]
 * @param param2 [Description of param2 and constraints]
 * @return [What this method returns and possible error codes]
 * @pre [List preconditions that caller must ensure]
 */
_Check_return_ ReturnType MethodName(
    _[SAL_ANNOTATION]_ ParameterType1 param1,
    _[SAL_ANNOTATION]_ ParameterType2 param2) noexcept;
```

**Implementation File (`.cpp`) Template:**
```cpp
/**
 * @details
 * [Explain how this method works internally.]
 * [Explain any performance considerations.]
 * [Mention validation logic for Weak contracts.]
 * [Mention memory management responsibilities.]
 * [Note any threading considerations.]
 */
ReturnType MethodName(
    ParameterType1 param1,
    ParameterType2 param2) noexcept
{
    // Implementation with appropriate assertions or validation
    // based on contract type
}
```

### SAL Annotation Selection Guide

| Contract Type | Parameter Safety | SAL Annotation Choice |
|---------------|------------------|----------------------|
| **Strong** | Caller guarantees non-null | `_In_`, `_Out_`, `_Inout_` |
| **Strong** | Caller guarantees valid array | `_In_reads_(count)` |
| **Weak** | Function accepts null | `_In_opt_`, `_Out_opt_` |
| **Weak** | Function validates array | `_In_reads_opt_(count)` |
| **Hybrid** | Mix of guaranteed and optional | Mix of `_In_` and `_In_opt_` |

---

## Why SAL Annotations Are Mandatory

SAL annotations provide:
* **Static analysis support** - Helps Visual Studio and SonarQube detect potential bugs
* **Documentation** - Makes function contracts explicit and self-documenting
* **Safety compliance** - Required for DO-178C Level A certification
* **Runtime safety** - Helps prevent buffer overruns, null pointer dereferences, and memory leaks

**All functions in this project MUST be SAL-annotated. No exceptions.**

---

## Contract Types and SAL Annotations

### Understanding Contract Types

Before applying SAL annotations, you must understand the three contract types from our Method Documentation Standards:

| Contract Type | Meaning | SAL Annotation Impact |
|---------------|---------|----------------------|
| **Strong** | Caller must ensure all preconditions are met. Function does not validate inputs. | Use non-optional annotations (`_In_`, `_Out_`) |
| **Weak** | Function validates all inputs and handles errors gracefully. | Use optional annotations where appropriate (`_In_opt_`) |
| **Hybrid** | Function validates some inputs but expects caller to meet other preconditions. | Mix of optional and non-optional annotations |

**The contract type directly influences which SAL annotations you choose.**

---

## Core SAL Annotation Categories

### 1. Parameter Direction Annotations

| Annotation | Meaning | When to Use |
|------------|---------|-------------|
| `_In_` | Input parameter, will be read | Parameter is read-only |
| `_Out_` | Output parameter, will be written | Parameter receives output value |
| `_Inout_` | Input/Output parameter | Parameter is both read and modified |

### 2. Pointer and String Annotations

| Annotation | Meaning | When to Use |
|------------|---------|-------------|
| `_In_opt_` | Optional input pointer (can be null) | Pointer parameter that may be null |
| `_Out_opt_` | Optional output pointer (can be null) | Output pointer that may be null |
| `_Outptr_` | Output pointer to pointer | Function allocates memory for caller |
| `_In_z_` | Input null-terminated string | String parameters |
| `_Out_z_cap_(size)` | Output string buffer with capacity | Output string with known buffer size |

### 3. Array and Buffer Annotations

| Annotation | Meaning | When to Use |
|------------|---------|-------------|
| `_In_reads_(count)` | Input array with count elements | Input arrays with known size |
| `_Out_writes_(count)` | Output array with count elements | Output arrays with known size |
| `_Inout_updates_(count)` | Input/output array with count elements | Arrays that are read and modified |

---

## Mandatory Annotation Rules

### Rule 1: All Function Parameters Must Be Annotated

SAL annotations must match the contract type specified in documentation.

**✅ CORRECT - Strong Contract:**
```cpp
/**
 * @brief Initialize the rendering system.
 * @contract Strong - Caller must ensure valid parameters.
 * @param deviceContext Pointer to D3D11 device context. Must not be null.
 * @param windowHandle Handle to the target window.
 * @return S_OK if successful, error code otherwise.
 * @pre deviceContext must not be null.
 */
HRESULT InitializeRenderer(
    _In_ ID3D11DeviceContext* deviceContext,
    _In_ HWND windowHandle) noexcept;
```

**✅ CORRECT - Weak Contract:**
```cpp
/**
 * @brief Initialize the rendering system with validation.
 * @contract Weak - Function validates all parameters and handles errors.
 * @param deviceContext Pointer to D3D11 device context. Can be null.
 * @param windowHandle Handle to the target window.
 * @return S_OK if successful, E_INVALIDARG for invalid parameters.
 * @pre None - function validates all inputs.
 */
HRESULT InitializeRendererSafe(
    _In_opt_ ID3D11DeviceContext* deviceContext,
    _In_ HWND windowHandle) noexcept;
```

**❌ INCORRECT:**
```cpp
// Missing SAL annotations AND contract documentation
HRESULT InitializeRenderer(
    ID3D11DeviceContext* deviceContext,
    HWND windowHandle) noexcept;
```

### Rule 2: Return Values Must Be Annotated When Appropriate

**✅ CORRECT - Strong Contract:**
```cpp
/**
 * @brief Create a new texture resource.
 * @contract Strong - Caller must ensure valid parameters.
 * @param width Texture width in pixels. Must be > 0.
 * @param height Texture height in pixels. Must be > 0.
 * @param textureOut Receives pointer to created texture.
 * @return S_OK if successful, error code otherwise.
 * @pre width > 0, height > 0, textureOut must not be null.
 */
_Check_return_ HRESULT CreateTexture(
    _In_ UINT width,
    _In_ UINT height,
    _Outptr_ ID3D11Texture2D** textureOut) noexcept;
```

**✅ CORRECT - Weak Contract:**
```cpp
/**
 * @brief Create a new texture resource with validation.
 * @contract Weak - Function validates all parameters and handles errors.
 * @param width Texture width in pixels.
 * @param height Texture height in pixels.
 * @param textureOut Receives pointer to created texture.
 * @return S_OK if successful, E_INVALIDARG for invalid parameters.
 * @pre textureOut must not be null.
 */
_Check_return_ HRESULT CreateTextureSafe(
    _In_ UINT width,
    _In_ UINT height,
    _Outptr_ ID3D11Texture2D** textureOut) noexcept;
```

### Rule 3: String Parameters Must Use String-Specific Annotations

**✅ CORRECT - Strong Contract:**
```cpp
/**
 * @brief Load shader from file.
 * @contract Strong - Caller must ensure valid file path.
 * @param shaderPath Path to shader file. Must be null-terminated and valid.
 * @param shaderOut Receives pointer to compiled shader.
 * @return S_OK if successful, error code otherwise.
 * @pre shaderPath and shaderOut must not be null, file must exist.
 */
_Check_return_ HRESULT LoadShaderFromFile(
    _In_z_ const wchar_t* shaderPath,
    _Outptr_ ID3D11VertexShader** shaderOut) noexcept;
```

**✅ CORRECT - Weak Contract:**
```cpp
/**
 * @brief Load shader from file with validation.
 * @contract Weak - Function validates file path and handles errors.
 * @param shaderPath Path to shader file. Can be null or invalid.
 * @param shaderOut Receives pointer to compiled shader.
 * @return S_OK if successful, E_INVALIDARG for bad path, E_FAIL for file errors.
 * @pre shaderOut must not be null.
 */
_Check_return_ HRESULT LoadShaderFromFileSafe(
    _In_opt_z_ const wchar_t* shaderPath,
    _Outptr_ ID3D11VertexShader** shaderOut) noexcept;
```

---

## Common SAL Annotation Patterns

### Pattern 1: DirectX Resource Creation

```cpp
/**
 * @brief Create vertex buffer from vertex data.
 * @contract Strong - Caller ensures valid vertex data and count.
 * @param vertices Array of vertex data. Must not be null.
 * @param vertexCount Number of vertices in the array.
 * @param vertexBufferOut Receives pointer to created buffer.
 * @return S_OK if successful, error code otherwise.
 * @pre vertices must not be null, vertexCount must be > 0.
 */
_Check_return_ HRESULT CreateVertexBuffer(
    _In_reads_(vertexCount) const Vertex* vertices,
    _In_ UINT vertexCount,
    _Outptr_ ID3D11Buffer** vertexBufferOut) noexcept;
```

### Pattern 2: Matrix and Vector Operations

```cpp
/**
 * @brief Transform vertices by matrix.
 * @contract Strong - Caller ensures valid arrays and count.
 * @param inputVertices Array of input vertices to transform.
 * @param outputVertices Array to receive transformed vertices.
 * @param vertexCount Number of vertices to transform.
 * @param transformMatrix Transformation matrix to apply.
 * @return Number of vertices successfully transformed.
 * @pre All parameters must not be null, vertexCount must be > 0.
 */
UINT TransformVertices(
    _In_reads_(vertexCount) const Vertex* inputVertices,
    _Out_writes_(vertexCount) Vertex* outputVertices,
    _In_ UINT vertexCount,
    _In_ const Matrix4x4* transformMatrix) noexcept;
```

### Pattern 3: Optional Parameters

```cpp
/**
 * @brief Set render target with optional depth buffer.
 * @contract Weak - Function accepts null depth buffer.
 * @param renderTarget Primary render target. Must not be null.
 * @param depthBuffer Optional depth buffer. Can be null.
 * @return S_OK if successful, error code otherwise.
 * @pre renderTarget must not be null.
 */
_Check_return_ HRESULT SetRenderTarget(
    _In_ ID3D11RenderTargetView* renderTarget,
    _In_opt_ ID3D11DepthStencilView* depthBuffer) noexcept;
```

### Pattern 4: String Buffers and Capacity

```cpp
/**
 * @brief Get shader error message.
 * @contract Weak - Function handles buffer size validation.
 * @param errorBuffer Buffer to receive error message.
 * @param bufferSize Size of error buffer in characters.
 * @param actualSize Receives actual size needed for message.
 * @return S_OK if successful, S_FALSE if buffer too small.
 * @pre errorBuffer must not be null if bufferSize > 0.
 */
_Check_return_ HRESULT GetShaderErrorMessage(
    _Out_z_cap_(bufferSize) wchar_t* errorBuffer,
    _In_ UINT bufferSize,
    _Out_opt_ UINT* actualSize) noexcept;
```

---

## SAL Annotations for Different Function Types

### Constructor/Destructor Patterns

```cpp
class RenderEngine
{
public:
    /**
     * @brief Constructor with required DirectX device.
     * @contract Strong - Caller must provide valid device.
     * @param device DirectX 11 device. Must not be null.
     * @pre device must not be null and properly initialized.
     */
    explicit RenderEngine(_In_ ID3D11Device* device) noexcept;

    /**
     * @brief Destructor - releases all resources.
     * @contract Strong - Safe to call multiple times.
     */
    ~RenderEngine() noexcept;

    // Explicitly deleted compiler-generated functions
    RenderEngine(const RenderEngine&) = delete;
    RenderEngine& operator=(const RenderEngine&) = delete;
    RenderEngine(RenderEngine&&) = delete;
    RenderEngine& operator=(RenderEngine&&) = delete;
};
```

### Member Function Patterns

```cpp
class TextureManager
{
public:
    /**
     * @brief Load texture from file with optional mip mapping.
     * @contract Weak - Function validates all parameters.
     * @param texturePath Path to texture file. Must be null-terminated.
     * @param generateMips Whether to generate mip maps automatically.
     * @param textureOut Receives pointer to loaded texture.
     * @return S_OK if successful, error code otherwise.
     * @pre texturePath and textureOut must not be null.
     */
    _Check_return_ HRESULT LoadTexture(
        _In_z_ const wchar_t* texturePath,
        _In_ bool generateMips,
        _Outptr_ ID3D11ShaderResourceView** textureOut) const noexcept;

private:
    ID3D11Device* mDevice;  // Non-owning pointer
};
```

---

## Error Handling with SAL Annotations

### HRESULT Return Pattern

```cpp
/**
 * @brief Compile shader from source code.
 * @contract Weak - Function validates input and provides detailed errors.
 * @param shaderSource Null-terminated shader source code.
 * @param shaderType Type of shader to compile (VS, PS, etc.).
 * @param compiledShader Receives pointer to compiled shader blob.
 * @param errorMessage Optional buffer for error messages.
 * @param errorMessageSize Size of error message buffer.
 * @return S_OK if successful, E_INVALIDARG for bad parameters, 
 *         E_FAIL for compilation errors.
 * @pre shaderSource must not be null.
 */
_Check_return_ HRESULT CompileShader(
    _In_z_ const char* shaderSource,
    _In_ ShaderType shaderType,
    _Outptr_ ID3DBlob** compiledShader,
    _Out_opt_z_cap_(errorMessageSize) char* errorMessage,
    _In_ UINT errorMessageSize) noexcept;
```

### Exception-Safe Pattern (Still Use SAL)

```cpp
/**
 * @brief Parse configuration file safely.
 * @contract Hybrid - Validates parameters but may throw on critical errors.
 * @param configPath Path to configuration file.
 * @param configOut Receives parsed configuration data.
 * @return true if successful, false if file not found.
 * @throws std::bad_alloc if out of memory.
 * @pre configPath and configOut must not be null.
 */
_Check_return_ bool ParseConfigFile(
    _In_z_ const wchar_t* configPath,
    _Out_ ConfigData* configOut);
```

---

## Advanced SAL Annotation Examples

### Complex Buffer Operations

```cpp
/**
 * @brief Update vertex buffer with new data.
 * @contract Strong - Caller ensures buffer size compatibility.
 * @param vertexBuffer Target vertex buffer to update.
 * @param newVertices Array of new vertex data.
 * @param vertexCount Number of vertices in newVertices array.
 * @param startVertex Starting vertex index for update.
 * @return S_OK if successful, error code otherwise.
 * @pre All parameters must be valid, startVertex + vertexCount must not exceed buffer size.
 */
_Check_return_ HRESULT UpdateVertexBuffer(
    _In_ ID3D11Buffer* vertexBuffer,
    _In_reads_(vertexCount) const Vertex* newVertices,
    _In_ UINT vertexCount,
    _In_ UINT startVertex) noexcept;
```

### Callback Function Annotations

```cpp
/**
 * @brief Register rendering callback function.
 * @contract Weak - Function validates callback pointer.
 * @param callback Function to call during rendering. Can be null to unregister.
 * @param userData User data to pass to callback. Can be null.
 * @return S_OK if successful, E_INVALIDARG if callback registration fails.
 */
_Check_return_ HRESULT RegisterRenderCallback(
    _In_opt_ RenderCallbackFunc callback,
    _In_opt_ void* userData) noexcept;

// Callback function type definition with SAL annotations
typedef void (*RenderCallbackFunc)(
    _In_ float deltaTime,
    _In_opt_ void* userData) noexcept;
```

---

## SAL Annotation Checklist for Code Reviews

### Before Submitting Code:

- [ ] **Every function has complete header documentation with @brief, @contract, @param, @return, @pre**
- [ ] **Every function implementation has @details explaining internal workings**
- [ ] **Contract type (Strong/Weak/Hybrid) is clearly specified and matches implementation**
- [ ] **Every function parameter is SAL-annotated**
- [ ] **SAL annotations match the contract type (Strong = non-optional, Weak = optional where appropriate)**
- [ ] **Return values use `_Check_return_` when appropriate**
- [ ] **String parameters use `_In_z_`, `_Out_z_cap_()`, etc.**
- [ ] **Array parameters use `_In_reads_()`, `_Out_writes_()`, etc.**
- [ ] **Optional parameters use `_opt_` variants**
- [ ] **Output pointers use `_Outptr_` or `_Out_`**
- [ ] **Const correctness is maintained with SAL annotations**

### Common Mistakes to Avoid:

- [ ] **Don't mismatch contract type with SAL annotations (Strong contract with `_opt_` annotations)**
- [ ] **Don't use generic `_In_` for strings - use `_In_z_`**
- [ ] **Don't forget `_Check_return_` for functions that can fail**
- [ ] **Don't use `_Out_` for input/output parameters - use `_Inout_`**
- [ ] **Don't mix SAL annotation styles within the same function**
- [ ] **Don't omit contract type documentation**
- [ ] **Don't write @details that contradict the contract type**

---

## Integration with Static Analysis Tools

Our project uses these tools that understand SAL annotations:

### Visual Studio Code Analysis
- Enables `/analyze` compiler flag
- Detects SAL annotation violations
- Reports potential buffer overruns and null pointer dereferences

### SonarQube Integration
- Validates SAL annotation completeness
- Ensures consistent annotation patterns
- Reports missing or incorrect annotations as code quality issues

### ReSharper C++
- Provides SAL annotation suggestions
- Validates annotation correctness
- Highlights potential safety issues

---

## Summary

SAL annotations are **mandatory** for all functions in this project and must be integrated with proper documentation. They provide:

1. **Static analysis support** for catching bugs early
2. **Self-documenting code** that's easier to understand
3. **Compliance** with DO-178C Level A requirements
4. **Runtime safety** through better compiler optimization
5. **Clear contracts** that specify function behavior and caller responsibilities

**Remember:** Every function must have:
- Complete header documentation (@brief, @contract, @param, @return, @pre)
- Implementation details (@details)
- Proper SAL annotations that match the contract type
- Consistent application of Strong/Weak/Hybrid contract patterns

This is not optional - it's a project requirement for code safety, quality, and maintainability.

---

## Quick Reference Card

```cpp
// Header documentation template:
/**
 * @brief [What the function does]
 * @contract [Strong/Weak/Hybrid] - [Contract explanation]
 * @param param [Parameter description and constraints]
 * @return [Return value and possible errors]
 * @pre [Caller preconditions]
 */

// Implementation documentation template:
/**
 * @details
 * [How the function works internally]
 * [Performance considerations]
 * [Validation logic for weak contracts]
 * [Memory management notes]
 */

// Most common SAL patterns for new developers:

// Strong contract - caller guarantees validity
_In_ const MyClass* input              // Input parameter (read-only)
_Out_ MyClass* output                  // Output parameter (write-only)  
_Inout_ MyClass* inout                 // Input/output parameter
_In_reads_(count) const Item* items    // Array input with count
_Outptr_ MyClass** allocated           // Output pointer allocation

// Weak contract - function accepts null/validates
_In_opt_ const MyClass* optional       // Optional input (can be null)
_In_opt_z_ const wchar_t* optString    // Optional string input
_In_reads_opt_(count) const Item* items // Optional array input

// Common patterns regardless of contract
_In_z_ const wchar_t* stringInput      // String input
_Check_return_ HRESULT MyFunction(...) // Function return checking
```

Use this reference while writing new code until both SAL annotations and documentation patterns become second nature.