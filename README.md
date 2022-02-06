# imcxx
+ C++17 wrapper around ImGui

# RAII scopes
with RAII, it will be easier to call some secondary functions like `Begin()` and `End()` or `BeginCombo()` and `EndCombo()` at the right time.

### examples:
```cpp
  imcxx::window hello_world_1{ "Hello world#1" };
  if (hello_world_1)
  {
      // ...
  }
  
  if (imcxx::window hello_world_2{ "Hello world#2" })
  {
      // ...
  }
  
  imcxx::window{ "Hello world#3" } << 
    []()
  {
      // ...
  };
```

# Shared operators
Since most if not all of the classes shares a class `scope_wrap`, they also share the same state. Meaning if the original function say `ImGui::Text` (returns void), was called with imcxx::text then it's state will always be active.
Some other functions like `ImGui::Begin`/`imcxx::window` which will only be active if their original function returned true.

### static call(...)
create the object from its parent's signature while forwarding the arguments. Useful when you don't want to name multiple objects at same time.

### operator bool()
Returns true if the function returned true or the function has a void return type, else false.

### operator<<(function fn)
Calls the function `fn` if the state was active.

### active_invoke(function fn, ...)
same as operator<<(fn), but forwards the arguments.

### inactive_invoke(function fn, ...)
Calls the function `fn` if the state wasn't active.

### passive_invoke(function fn, ...)
Calls the function `fn` regardless of it's state.

### Move constructor
Move the return value from the original object to the new one, and invalidate it.


# Notes:
Credits to `ocornut` for [ImGui](https://github.com/ocornut/imgui/tree/docking)
More examples can be under `test/*/main.cpp` and `include/imgui/imgui_demo.cpp` (Not completed yet)
