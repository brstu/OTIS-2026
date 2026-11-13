# Task 02 — Advanced Unit Testing & Code Coverage

## Project structure

- `src/main.cpp` — updated console application; imports the original models from `../task_01/scr`.
- `src/stability_analyzer.*` — analytic/local stability checks.
- `test/` — Google Test unit tests, including parameterized `TEST_P` suites.
- `run_tests.bat` — local Windows build, CTest execution and optional gcovr reports.

## Reuse of Task 01 code

The model implementations are **not copied** into Task 02. CMake builds `task_01/scr/model11.cpp`, `model28.cpp`, and `model36.cpp` directly and includes their original headers. This avoids duplicate implementations and makes Task 02 tests exercise the actual Task 01 model classes.

## Stability analysis

### Model 1.1

For `y[k+1] = a*y[k] + b*u[k]`, the homogeneous recurrence has characteristic equation `z - a = 0`; its only root is `z=a`. Asymptotic stability requires `|a| < 1`. At `|a|=1` the system is marginal and is not asymptotically stable; for `|a|>1` it is unstable.

### Model 2.8

This is nonlinear, so a global LTI pole test is not applicable. Around the zero equilibrium (`y=0`, `y_prev=0`, `u=0`), linearization in the current state gives `dF/dy = a`; the local first-order criterion is therefore `|a| < 1`. This is a local criterion only and is not a proof of global stability.

### Model 3.6

For `dy/dt = a*y^3 + b*u`, the explicit Euler update is `y[k+1]=y[k]+dt*(a*y[k]^3+b*u[k])`. At the zero-input equilibrium, the derivative of `a*y^3` at `y=0` is zero. The linearized discrete root is `z=1`, so linearization does not establish asymptotic stability. The application therefore prints a caution rather than claiming the nonlinear model is globally unstable.

## Tests and coverage

Test suites cover known numeric outputs for Models 1.1, 2.8 and 3.6, Euler steps, reset and zero-input invariants, step/pulse/harmonic input injection, stability boundaries, and warning generation. Parameterized `TEST_P` suites vary coefficients and inputs without duplicating test logic.

- **Test definitions:** 11 (parameterized suites expand into multiple test instances; exact CTest count is printed during execution).
- **Test pass count:** generated locally by `ctest` in `test_report.xml`.
- **Coverage:** generated locally by `gcovr` as `coverage.xml` and `coverage.html` when gcovr is installed. No fabricated percentage is recorded in source control; run the batch script to obtain measurements for the local compiler/build.

## Build and run

Requirements: CMake, MinGW-w64 with GCC, Python, and internet access on the first build (GoogleTest is fetched by CMake). Optional: `gcovr` (`py -m pip install gcovr`).

Run `run_tests.bat` from this directory. The script pauses at the end on both success and failure, so the console stays open and the first compiler/CMake error remains visible. It configures the project, builds the application and tests, runs CTest and creates coverage reports when gcovr is available.

To run the simulator manually: `build\\task02.exe`.
