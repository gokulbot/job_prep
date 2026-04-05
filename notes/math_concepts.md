# Math Concepts

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
