# Math Concepts

---

## M1 — Kalman Filter: Position-Velocity State

### State & Models

State vector:
```
x_k = [p_k; v_k]
```

**Process model:**
```
x_k = F * x_{k-1} + B * u_k + w_k

F = [1, dt; 0, 1]       (state transition — kinematics)
B = [0.5*dt^2; dt]      (input matrix — acceleration input)
u_k = a_k               (control input — acceleration)
w_k ~ N(0, Q)           (process noise)
```

**Observation model** (position only):
```
z_k = H * x_k + n_k
H = [1, 0]
n_k ~ N(0, R)
```

---

### Process Noise Covariance Q

Q is NOT the encoder/sensor noise. It captures how much the motion model is wrong —
unmodeled disturbances, friction, slipping, external forces.

For a white-noise acceleration model (sigma_a = std dev of acceleration noise):
```
Q = B * sigma_a^2 * B^T
  = [0.25*dt^4, 0.5*dt^3; 0.5*dt^3, dt^2] * sigma_a^2
```

This is covariance propagation: J * sigma^2 * J^T where J = B.

**Q vs R:**
- Q — distrust in the motion model → used in predict step
- R — distrust in the sensor → used in update step

---

### Predict Step

```
x_hat_{k|k-1} = F * x_hat_{k-1|k-1} + B * u_k
P_{k|k-1}     = F * P_{k-1|k-1} * F^T + Q
```

P_{k|k-1} has two parts:
- F * P * F^T — prior uncertainty propagated through motion model
- Q — new uncertainty added by the motion model itself

---

### Update Step — Kalman Gain Derivation (MSE)

**Key idea:** assume the corrected estimate is linear in the innovation:
```
x_hat_{k|k} = x_hat_{k|k-1} + K * (z_k - H * x_hat_{k|k-1})
```

Innovation = z_k - H * x_hat_{k|k-1} (difference between actual and predicted measurement).
K is like a proportional controller gain — scales how much you trust the measurement vs prediction.

**Why squared error?** Because noise is Gaussian → log likelihood is quadratic → minimizing
MSE = maximizing likelihood. Not an arbitrary choice.

**Derivation:**

Define prior error: e_prior = x_k - x_hat_{k|k-1}

Posterior error:
```
e_k = x_k - x_hat_{k|k}
    = x_k - x_hat_{k|k-1} - K*(z_k - H*x_hat_{k|k-1})
```

Substitute z_k = H*x_k + n_k:
```
innovation = z_k - H*x_hat_{k|k-1} = H*e_prior + n_k
```

So:
```
e_k = (I - K*H)*e_prior - K*n_k
```

Posterior covariance P_{k|k} = E[e_k * e_k^T].

Expand using (a-b)(a-b)^T = a*a^T - a*b^T - b*a^T + b*b^T:
```
P_{k|k} = (I-KH)*E[e_prior*e_prior^T]*(I-KH)^T
         + K*E[n_k*n_k^T]*K^T
         - cross terms (= 0)
```

Cross terms drop because e_prior and n_k are independent and zero mean:
- Linearity of expectation: E[A*X*B] = A*E[X]*B for constant A, B
- Independence + zero mean: E[e_prior * n_k^T] = E[e_prior]*E[n_k^T] = 0

So:
```
P_{k|k} = (I-KH) * P_{k|k-1} * (I-KH)^T + K*R*K^T
```

**Minimize trace(P_{k|k}) w.r.t K:**

Why trace? P_{k|k} is a matrix, can't minimize directly. Trace = sum of variances = total MSE.
Off-diagonal terms are consequences of K — once K is fixed, the full matrix is determined.

Using matrix derivative rules:
```
d/dK trace(K*A*K^T) = 2*K*A    (A symmetric)
d/dK trace(K*B)     = B^T
```

Setting derivative to zero:
```
-2*P_{k|k-1}*H^T + 2*K*(H*P_{k|k-1}*H^T + R) = 0
```

Solving:
```
K_k = P_{k|k-1} * H^T * (H*P_{k|k-1}*H^T + R)^-1
```

Intuition:
- P_{k|k-1}*H^T — prior uncertainty projected onto measurement space
- (H*P_{k|k-1}*H^T + R) — total uncertainty in measurement space (prior + sensor noise)
- Large R (noisy sensor) → small K → trust prediction more
- Large P (uncertain prediction) → large K → trust measurement more

---

### Corrected Covariance

Simplified form:
```
P_{k|k} = (I - K_k*H) * P_{k|k-1}
```

**Numerically unstable** — errors in K_k (from ill-conditioned inverse) can make P lose
symmetry and positive semi-definiteness.

Joseph form (stable, always use in practice):
```
P_{k|k} = (I - K_k*H) * P_{k|k-1} * (I - K_k*H)^T + K_k*R*K_k^T
```

---

### Full KF Cycle Summary

**Predict:**
```
x_hat_{k|k-1} = F * x_hat_{k-1|k-1} + B * u_k
P_{k|k-1}     = F * P_{k-1|k-1} * F^T + Q
```

**Update:**
```
K_k           = P_{k|k-1} * H^T * (H*P_{k|k-1}*H^T + R)^-1
x_hat_{k|k}   = x_hat_{k|k-1} + K_k * (z_k - H*x_hat_{k|k-1})
P_{k|k}       = (I - K_k*H) * P_{k|k-1}           (simplified, numerically unstable)
P_{k|k}       = (I-KH)*P_{k|k-1}*(I-KH)^T + K*R*K^T  (Joseph form, use this)
```

---

## M2 — Inverse Depth Parameterization

### Setup

In monocular SLAM, depth `d` is reparameterized as inverse depth `ρ = 1/d`.

Given:
```
d ~ N(μ_d, σ_d²)     (approximation valid when σ_d << μ_d)
ρ = g(d) = 1/d
```

---

### Task 1 — Change of Variables: PDF of ρ

To find the PDF of `ρ = 1/d`, apply the change of variables formula:

```
f_ρ(ρ) = f_d(g⁻¹(ρ)) × |d/dρ (g⁻¹(ρ))|
```

Where:
- `g⁻¹(ρ) = 1/ρ`
- `d/dρ (1/ρ) = -1/ρ²`  →  absolute value = `1/ρ²`

So:
```
f_ρ(ρ) = f_d(1/ρ) × (1/ρ²)
```

Substituting the Gaussian PDF of `d`:
```
f_ρ(ρ) = [1/(√(2π)σ_d)] × exp(-(1/ρ - μ_d)² / 2σ_d²) × (1/ρ²)
```

This is the **reciprocal normal** distribution.

---

### Task 2 — Identify the Distribution

`ρ = 1/d` where `d ~ Normal` follows a **reciprocal normal distribution**.

**Important distinction:**
| Name | What it is |
|------|-----------|
| Normal / Gaussian | Bell curve, symmetric — same thing, different names |
| Reciprocal Normal | Distribution of `1/x` where `x ~ Normal` |
| Inverse Gaussian | First passage time of Brownian motion — completely unrelated despite the name |

The reciprocal normal is:
- **Not symmetric** — heavy tail on one side
- **Not Gaussian** — which is why we need approximations (delta method) for EKF

---

### Task 3 — Delta Method: Mean and Variance of ρ

The delta method uses a first-order Taylor expansion to propagate uncertainty through a nonlinear function.

**Taylor expansion of `g(d) = 1/d` around `μ_d`:**
```
ρ ≈ g(μ_d) + g'(μ_d)(d - μ_d)
  = 1/μ_d + (-1/μ_d²)(d - μ_d)
```

**Mean:**

Take expectation of both sides:
```
E[ρ] ≈ E[1/μ_d] + (-1/μ_d²) × E[d - μ_d]
      = 1/μ_d    + (-1/μ_d²) × 0
      = 1/μ_d
```

The second term vanishes because `E[d - μ_d] = E[d] - μ_d = 0` by definition of mean.

**Key insight:** for any first-order Taylor approximation (linear in input), the mean of the output = function evaluated at the mean of the input. The correction term always drops out.

**Variance:**

The Jacobian is `J = g'(μ_d) = -1/μ_d²`

Using the covariance propagation formula `Σ_y = J Σ_x Jᵀ`:
```
Var[ρ] = J² × Var[d]
       = (1/μ_d²)² × σ_d²
       = σ_d² / μ_d⁴
```

**Covariance propagation formula (general):**
```
Σ_y = J Σ_x Jᵀ
```
Where `J = dg/dx` evaluated at the mean. For scalar case, `J² × σ²`. For vector case, full matrix multiplication.

Raw variance definition:
```
Var[x] = E[(x - μ)²] = Σ (xᵢ - μ)² p(xᵢ)   (discrete)
                      = ∫ (x - μ)² f(x) dx      (continuous)
```

**Validity:** the approximation holds when `σ_d << μ_d` — i.e. uncertainty is small relative to the mean depth.

---

### Task 4 — Why Inverse Depth is Preferred

**Depth error is proportional to depth:**

For a far-away point at `d = 100m`, depth uncertainty could span `50m to ∞` — heavily skewed, non-Gaussian, heavy right tail. EKF assumes Gaussian noise, so this is a bad fit.

In inverse depth space:
```
Var[ρ] = σ_d² / μ_d⁴
```
As `d` gets large (far point), `μ_d⁴` grows fast → `Var[ρ]` shrinks. The uncertainty becomes small and symmetric near `ρ ≈ 0` — much more Gaussian-like → EKF linearization error is much smaller.

**Sensor physics — the strongest argument:**

Stereo sensors and cameras directly measure **disparity**:
```
disparity = baseline × f / d  ∝  1/d  =  ρ
```

So:
```
Sensor measures disparity ~ Gaussian     (sensor noise is Gaussian in disparity)
Disparity ∝ 1/d = ρ
→ ρ ~ Gaussian  ✓   correct model
→ d ~ Reciprocal Normal  ✗   wrong model for EKF
```

The sensor is **natively Gaussian in inverse depth space** — modeling noise as Gaussian in depth is the wrong parameterization from the start. Inverse depth is not just a mathematical trick — it matches the sensor physics.

---

### Task 5 — 3D Extension: (θ, φ, ρ) Parameterization

A 3D point in monocular SLAM is parameterized in **camera coordinates** as `(θ, φ, ρ)`:
- `θ` — azimuth angle (horizontal bearing)
- `φ` — elevation angle (vertical bearing)
- `ρ = 1/d` — inverse depth

**Point in Cartesian camera coordinates:**
```
P = (1/ρ) × [cos(φ)sin(θ), sin(φ), cos(φ)cos(θ)]ᵀ

X = cos(φ)sin(θ) / ρ
Y = sin(φ) / ρ
Z = cos(φ)cos(θ) / ρ
```

**Jacobian J = dP/d(θ, φ, ρ) — 3×3 matrix:**

Column 1 — ∂P/∂θ:
```
∂X/∂θ =  cos(φ)cos(θ) / ρ
∂Y/∂θ =  0
∂Z/∂θ = -cos(φ)sin(θ) / ρ
```

Column 2 — ∂P/∂φ:
```
∂X/∂φ = -sin(φ)sin(θ) / ρ
∂Y/∂φ =  cos(φ) / ρ
∂Z/∂φ = -sin(φ)cos(θ) / ρ
```

Column 3 — ∂P/∂ρ:
```
∂X/∂ρ = -cos(φ)sin(θ) / ρ²
∂Y/∂ρ = -sin(φ) / ρ²
∂Z/∂ρ = -cos(φ)cos(θ) / ρ²
```

**Covariance propagation:**
```
Σ_cartesian = J × diag(σ_θ², σ_φ², σ_ρ²) × Jᵀ
```

---

### Generic Covariance Propagation Derivation

Given `y = f(x)`, `x ~ N(μ_x, Σ_x)`:

**Step 1 — Taylor expand around mean:**
```
y ≈ f(μ_x) + J(x - μ_x)
```
Where `J = df/dx` evaluated at `μ_x`

**Step 2 — Mean:**
```
E[y] ≈ f(μ_x)
```
Second term drops because `E[x - μ_x] = 0` by definition of mean.

**Step 3 — Covariance:**
```
Σ_y = E[(y - μ_y)(y - μ_y)ᵀ]
    = E[J(x - μ_x)(x - μ_x)ᵀ Jᵀ]
    = J E[(x - μ_x)(x - μ_x)ᵀ] Jᵀ
    = J Σ_x Jᵀ
```

**Why J comes outside the expectation:** `J` is a constant matrix (evaluated at the mean, not a random variable) — constants pull out of expectations, same as pulling out of integrals:
```
E[Ax] = ∫ Ax f(x) dx = A ∫ x f(x) dx = A E[x]
```

**Final formula:**
```
Σ_y = J Σ_x Jᵀ
```

This is used everywhere in EKF, SLAM, sensor fusion — any time you propagate uncertainty through a nonlinear function.

---

## M4 — Pinhole Camera Model

### Full Projection Pipeline

```
P_w -> [R | t] -> P_c -> divide by Z -> x_n -> distort -> x_d -> K -> pixel
```

### Geometric Projection (similar triangles)

```
x = f * X/Z
y = f * Y/Z
```

Z appears in denominator — perspective division (farther points appear smaller).

### Pixel Coordinates

```
u = fx * (X/Z) + cx
v = fy * (Y/Z) + cy
```

fx, fy — independent focal lengths for non-square pixels
cx, cy — principal point (where optical axis hits the sensor)

### Homogeneous Form

```
lambda * [u, v, 1]^T = K * [X, Y, Z]^T,   lambda = Z
```

Intrinsic matrix K:
```
K = | fx  s  cx |
    |  0  fy  cy |
    |  0   0   1 |
```

s = skew parameter (axis skew, ~0 in modern cameras)

### Full Pipeline with Extrinsics

World point P_w_tilde = [X, Y, Z, 1]^T (homogeneous — the appended 1 lets translation act on the point):

```
lambda * [u, v, 1]^T = K * [R | t] * [X, Y, Z, 1]^T
```

Projection matrix P = K * [R | t]:
```
    | fx  s  cx |   | r11 r12 r13  t1 |
P = |  0  fy  cy | * | r21 r22 r23  t2 |   (3x4)
    |  0   0   1 |   | r31 r32 r33  t3 |
```

- R is 3x3, t is 3x1, [R | t] is 3x4
- K is 3x3, P is 3x4

### Jacobian of Projection

2x3 Jacobian of [u, v] w.r.t. [X, Y, Z] in camera frame:

```
J = | fx/Z    0    -fx*X/Z^2 |
    |  0    fy/Z   -fy*Y/Z^2 |
```

Derived by partial differentiation of u = fx*X/Z + cx and v = fy*Y/Z + cy.
The -X/Z^2 and -Y/Z^2 terms come from quotient rule on X/Z and Y/Z.
Used in bundle adjustment and EKF-SLAM measurement updates.

### Distortion

Applied in normalized coordinates (after divide by Z, before K) — distortion is a lens property, independent of pixel size/resolution (those are captured by K).

Radial distortion:
```
x_d = x_n * (1 + k1*r^2 + k2*r^4)
y_d = y_n * (1 + k1*r^2 + k2*r^4)
r^2 = x_n^2 + y_n^2
```

k1 > 0 → barrel distortion (edges bow outward)
k1 < 0 → pincushion distortion (edges bow inward)

Final pixel:
```
u = fx * x_d + cx
v = fy * y_d + cy
```

---

## M30 — Solving Ax = b: Four Subspaces, All Cases

### The Two Sides of Ax = b

**b side (R^m) — reachability:**
```
C(A)    — col space   — b's A can reach    → solution exists
N(A^T)  — left null   — b's A can't reach  → no solution
dims: r + (m-r) = m
```

**x side (R^n) — visibility:**
```
C(A^T)  — row space   — part of x A can see    → produces output
N(A)    — null space  — part of x A kills      → Ax = 0
dims: r + (n-r) = n
```

Orthogonality: C(A^T) ⊥ N(A) and C(A) ⊥ N(A^T).

Any x splits as: `x = x_rowspace + x_null`. Only x_rowspace matters — A kills x_null.

---

### Four Cases

**Case 1 — Square invertible (m = n, r = n):**
```
x = A^-1 b
```
Exists iff det(A) ≠ 0 (full rank). Unique solution.

---

**Case 2 — Homogeneous Ax = 0:**
```
Solution set = null space N(A)
```
- r = n (full col rank): only trivial solution x = 0
- r < n: infinite solutions, dim = n - r free variables

For robotics (DLT, triangulation) — solve min ||Ax|| s.t. ||x|| = 1:
- Need unit norm constraint to avoid trivial x = 0
- Solution = last column of V from SVD of A (right singular vector of smallest singular value)
- Intuition: smallest singular value direction gets squished most by A → makes Ax closest to 0

---

**Case 3 — Overdetermined (m > n, b not in C(A)):**

No exact solution. Minimize ||Ax - b||^2.

Derivation — expand and differentiate w.r.t x:
```
(Ax - b)^T(Ax - b) = x^T A^T A x - 2 x^T A^T b + b^T b
d/dx = 2 A^T A x - 2 A^T b = 0
```

Normal equations:
```
A^T A x = A^T b
x = (A^T A)^-1 A^T b = A^+ b
```

Geometric meaning: b = b_col + b_leftnull. Least squares finds x that reaches b_col (projection onto C(A)). Residual b_leftnull lies in N(A^T) — irreducible error.

Matrix derivative rules used:
```
d/dx (x^T B x) = 2Bx      (B symmetric)
d/dx (x^T c)   = c
d/dx (c^T x)   = c
```

---

**Case 4 — Underdetermined (m < n):**

Infinite exact solutions. General solution:
```
x = x_particular + x_null    (x_null is any vector in N(A))
```

Minimum norm solution — set x_null = 0:
```
x_min = A^T (A A^T)^-1 b
```

Lies purely in row space C(A^T). Why minimum norm?
```
||x||^2 = ||x_row||^2 + ||x_null||^2    (orthogonal, so Pythagoras applies)
```
Set x_null = 0 → minimum. x_row is fixed by b, x_null is free.

Formula `A^T (A A^T)^-1 b` always lands in row space because A^T * (anything) is always in C(A^T).

---

### Pseudoinverse via SVD

A = U Σ V^T (U: mxr, Σ: rxr, V: nxr)

```
A^+ = V Σ^+ U^T
```

Where Σ^+ inverts non-zero singular values: diag(1/σ1, ..., 1/σr).

Why V and U^T? A maps R^n → R^m via: V^T (rotate in R^n) → Σ (scale) → U (rotate in R^m). Pseudoinverse reverses: U^T → Σ^+ → V.

Unifies all cases:
```
Square invertible  → A^+ = A^-1
Overdetermined     → A^+ gives least squares solution
Underdetermined    → A^+ gives minimum norm solution
```

Rank deficiency check: if σ_r / σ_1 < ε, treat σ_r as zero.

---

### Robotics Examples

```
DLT homography (noisy)    → Case 3 — overdetermined, least squares
Triangulation (noisy)     → Case 2 — homogeneous, SVD (min ||Ax|| s.t. ||x||=1)
VINS initialization       → Case 3 — overdetermined, least squares
Manifold opt (SO(3))      → Case 4 — underdetermined, use Lie algebra to avoid it
SLAM gauge freedom        → null space in info matrix (global pose unobservable)
                             fix gauge = anchor one pose = remove null space = solvable
```

**Triangulation vs DLT:** both minimize error but different structure:
- DLT: minimize ||Ax - b|| (inhomogeneous)
- Triangulation: minimize ||Ax|| s.t. ||x||=1 (homogeneous — need constraint to avoid x=0)

---

## M33 — Matrix A That Maps All x onto a Line

### Problem
Find 3x3 matrix A such that Ax lies on line span{[2,1,2]^T} for all x.

### Key Insight
Ax always lands on a line iff col space of A = that line → A must be **rank-1**.

### Solution — Outer Product Form
All columns of A must be proportional to b = [2,1,2]^T:

```
A = b * c^T   for any nonzero c = [c1, c2, c3]^T
```

Then: Ax = b * c^T * x = (c^T x) * b — always a scalar multiple of b.

Simplest example (c = [1,0,0]^T):
```
A = | 2  0  0 |
    | 1  0  0 |
    | 2  0  0 |
```

### Connection to Subspaces
- rank(A) = 1
- C(A) = span{b} — only one direction reachable
- N(A) has dim = 2 — most of R^3 gets killed

---
