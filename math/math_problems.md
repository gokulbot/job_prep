# Math & Robotics Derivation Problems

A running list of formalized derivation problems. Work through each, write the full derivation by hand or in LaTeX.

---

## M1 — Kalman Filter: Position-Velocity State

### Problem Statement
A rigid body moves in 1D (extend to 3D by stacking). Formalize the Kalman Filter process model and observation model, then derive the full KF update equations.

### State Definition
$$\mathbf{x}_k = \begin{bmatrix} p_k \\ v_k \end{bmatrix} \in \mathbb{R}^2$$

where $p_k$ is position and $v_k$ is velocity at timestep $k$, with sampling period $\Delta t$.

### Tasks
1. **Process model** — Write the discrete-time linear motion model $\mathbf{x}_{k} = F\mathbf{x}_{k-1} + B\mathbf{u}_k + \mathbf{w}_k$. Derive the state transition matrix $F$ and input matrix $B$ from the kinematic equations $p_k = p_{k-1} + v_{k-1}\Delta t + \frac{1}{2}a\Delta t^2$, $v_k = v_{k-1} + a\Delta t$. Assume process noise $\mathbf{w}_k \sim \mathcal{N}(0, Q)$ — derive $Q$ for a continuous white-noise acceleration model.

2. **Observation model** — Assume only position is measured: $z_k = Hp_k + \mathbf{v}_k$, $\mathbf{v}_k \sim \mathcal{N}(0, R)$. Write $H$.

3. **Predict step** — Derive:
$$\hat{\mathbf{x}}_{k|k-1} = F\hat{\mathbf{x}}_{k-1|k-1} + B\mathbf{u}_k$$
$$P_{k|k-1} = FP_{k-1|k-1}F^\top + Q$$

4. **Update step** — Derive the Kalman gain $K_k$, corrected state $\hat{\mathbf{x}}_{k|k}$, and corrected covariance $P_{k|k}$ by minimizing the mean squared error (or equivalently, by completing the square in the joint Gaussian).

5. **Extension** — Generalize to 3D: $\mathbf{x} \in \mathbb{R}^6 = [x, y, z, \dot{x}, \dot{y}, \dot{z}]^\top$.

### Key Quantities to Derive
- $F$, $B$, $H$, $Q$ (white-noise acceleration model), $K_k$
- Why $P_{k|k} = (I - K_k H) P_{k|k-1}$ and when this formula is numerically unstable

---

## M2 — Inverse Depth Parameterization: Distribution Derivation ✅

### Problem Statement
In monocular SLAM and visual odometry, depth $d$ is often reparameterized as inverse depth $\rho = 1/d$. Derive what distribution $\rho$ follows if $d$ follows a Gaussian, and motivate why inverse depth is preferred.

### Setup
Let depth $d > 0$ with $d \sim \mathcal{N}(\mu_d, \sigma_d^2)$ (approximation valid when $\sigma_d \ll \mu_d$).  
Define $\rho = g(d) = \frac{1}{d}$, a monotone decreasing bijection on $(0, \infty)$.

### Tasks
1. **Exact distribution via change of variables** — Apply the formula:
$$f_\rho(\rho) = f_d(g^{-1}(\rho)) \left|\frac{d}{d\rho} g^{-1}(\rho)\right|$$
where $g^{-1}(\rho) = 1/\rho$. Write the resulting PDF explicitly.

2. **Identify the distribution** — Show that $\rho = 1/d$ follows a **reciprocal normal** (Inverse Gaussian is different — be precise). Sketch the shape and compare to $\mathcal{N}(\mu_d^{-1}, ?)$.

3. **First-order Gaussian approximation (delta method)** — Approximate the mean and variance of $\rho$ using a first-order Taylor expansion of $g(d) = 1/d$ around $\mu_d$:
$$\mathbb{E}[\rho] \approx \frac{1}{\mu_d}, \quad \text{Var}[\rho] \approx \left(\frac{1}{\mu_d^2}\right)^2 \sigma_d^2 = \frac{\sigma_d^2}{\mu_d^4}$$
Derive this. State when the approximation is valid.

4. **Motivation** — Explain why a far-away point (large $d$, small $\rho$) has a more symmetric, well-conditioned uncertainty in $\rho$ than in $d$. Why does the EKF linearization error shrink in inverse depth space?

5. **Extension** — In the MSCKF / SVO formulation, a 3D point is parameterized as $(\theta, \phi, \rho)$ where $\theta, \phi$ are bearing angles and $\rho = 1/d$. Write the full 3D point in camera coordinates and propagate a diagonal covariance $\Sigma = \text{diag}(\sigma_\theta^2, \sigma_\phi^2, \sigma_\rho^2)$ to Cartesian uncertainty via the Jacobian.

---

## M3 — Pose & Uncertainty Propagation: IMU Frame → Camera Frame

### Problem Statement
An IMU estimates its pose (position + orientation) with some uncertainty. The camera is rigidly attached to the IMU with a known extrinsic calibration. Formalize the propagation of both the state (pose) and its covariance from the IMU frame to the camera frame.

### Setup
- IMU pose in world: $T_{WI} = (R_{WI}, \mathbf{p}_{WI}) \in SE(3)$, with uncertainty $\Sigma_I \in \mathbb{R}^{6\times 6}$ (in the Lie algebra $\mathfrak{se}(3)$ or split as $\Sigma_R \in \mathbb{R}^{3\times 3}$, $\Sigma_p \in \mathbb{R}^{3\times 3}$).
- Extrinsic calibration (known, no uncertainty): $T_{IC} = (R_{IC}, \mathbf{p}_{IC})$ — rigid transform from camera to IMU.
- Goal: find $T_{WC}$ and $\Sigma_C$.

### Tasks
1. **State propagation** — Derive $T_{WC}$ from $T_{WI}$ and $T_{IC}$:
$$T_{WC} = T_{WI} \cdot T_{IC}$$
Expand explicitly:
$$R_{WC} = R_{WI} R_{IC}$$
$$\mathbf{p}_{WC} = \mathbf{p}_{WI} + R_{WI} \mathbf{p}_{IC}$$

2. **Jacobian derivation** — Define the error state $\delta\xi_I = [\delta\phi_I, \delta\mathbf{p}_I]^\top \in \mathbb{R}^6$ (rotation error in $\mathfrak{so}(3)$, position error). Compute the Jacobian $J = \frac{\partial \xi_C}{\partial \xi_I}$ where $\xi_C = [\delta\phi_C, \delta\mathbf{p}_C]^\top$. Show:
$$\delta\phi_C = R_{IC}^\top \delta\phi_I$$
$$\delta\mathbf{p}_C = R_{IC}^\top (\delta\mathbf{p}_I - \lfloor\delta\phi_I\rfloor_\times \mathbf{p}_{IC})$$
where $\lfloor\cdot\rfloor_\times$ is the skew-symmetric matrix.

3. **Covariance propagation** — Apply:
$$\Sigma_C = J \Sigma_I J^\top$$
Write $J$ in block form and expand $\Sigma_C$ explicitly.

4. **Numerical check** — Describe a Monte Carlo validation: sample $N=10^4$ perturbations from $\Sigma_I$, transform each via $T_{WC}$, compute sample covariance, compare to $J\Sigma_I J^\top$.

5. **Extension** — If $T_{IC}$ is also uncertain (calibration error $\Sigma_{IC}$), write the full Jacobian with respect to both $\xi_I$ and $\xi_{IC}$ and propagate the combined uncertainty.

---

## M4 — Pinhole Camera Projection Equation

### Problem Statement
Derive the full projection equation for a pinhole camera — from a 3D world point to a 2D pixel — including the intrinsic matrix, homogeneous coordinates, and the role of each parameter.

### Setup
- 3D point in camera frame: $\mathbf{P}_c = [X, Y, Z]^\top \in \mathbb{R}^3$, with $Z > 0$ (in front of camera)
- Focal length: $f$ (or $f_x, f_y$ for non-square pixels)
- Principal point: $(c_x, c_y)$ — pixel coordinates of the optical axis
- Image plane at distance $f$ from the optical center

### Tasks
1. **Geometric derivation** — Using similar triangles on the optical axis, show that a point $(X, Y, Z)$ in camera coordinates projects to:
$$x = f \frac{X}{Z}, \quad y = f \frac{Y}{Z}$$
Draw the diagram. Explain why $Z$ appears in the denominator (perspective division).

2. **Pixel coordinates** — Account for the principal point offset and independent focal lengths:
$$u = f_x \frac{X}{Z} + c_x, \quad v = f_y \frac{Y}{Z} + c_y$$

3. **Homogeneous form** — Rewrite using homogeneous coordinates. Show that the projection can be written as:
$$\lambda \begin{bmatrix} u \\ v \\ 1 \end{bmatrix} = K \begin{bmatrix} X \\ Y \\ Z \end{bmatrix}, \quad \lambda = Z$$
where the intrinsic matrix $K$ is:
$$K = \begin{bmatrix} f_x & s & c_x \\ 0 & f_y & c_y \\ 0 & 0 & 1 \end{bmatrix}$$
Explain the skew parameter $s$ (axis skew — usually $\approx 0$ in modern cameras).

4. **Full pipeline** — Chain the world-to-camera transform with projection. Given a world point $\mathbf{P}_w$ and extrinsics $[R | \mathbf{t}]$:
$$\lambda \tilde{\mathbf{p}} = K [R | \mathbf{t}] \tilde{\mathbf{P}}_w$$
where $\tilde{\cdot}$ denotes homogeneous form. Write the full $3 \times 4$ projection matrix $P = K[R|\mathbf{t}]$.

5. **Jacobian** — Derive $\frac{\partial [u, v]^\top}{\partial [X, Y, Z]^\top}$, the $2\times 3$ Jacobian of the projection with respect to the 3D point. This is used in bundle adjustment and EKF-SLAM measurement updates.

6. **Distortion** — Describe (don't fully derive) how radial distortion modifies the ideal projection:
$$u_d = u(1 + k_1 r^2 + k_2 r^4), \quad r^2 = (u - c_x)^2 + (v - c_y)^2$$
Why does distortion need to be applied before or after the linear projection?

---

---

## M5 — Camera Distortion: Full Model & Jacobian Derivation

### Problem Statement
Derive the complete distortion-aware projection pipeline, including radial and tangential distortion models, and derive the Jacobian of the full projection for use in EKF and bundle adjustment.

### Setup
- 3D point in camera frame: $\mathbf{P}_c = [X, Y, Z]^\top$
- Intrinsics: $K = \text{diag}(f_x, f_y, 1)$ with principal point $(c_x, c_y)$
- Distortion coefficients: $k_1, k_2, k_3$ (radial), $p_1, p_2$ (tangential)

### Tasks

1. **Pipeline order** — Write the four-stage pipeline and justify why distortion is applied in normalized coordinates (after perspective divide, before applying $K$):
$$\mathbf{P}_c \xrightarrow{\div Z} \mathbf{x}_n \xrightarrow{\text{distort}} \mathbf{x}_d \xrightarrow{K} \mathbf{p}$$
Explain why distortion is a lens property independent of pixel size/resolution.

2. **Radial distortion** — Let $r^2 = x_n^2 + y_n^2$. Derive:
$$x_d = x_n(1 + k_1 r^2 + k_2 r^4 + k_3 r^6)$$
$$y_d = y_n(1 + k_1 r^2 + k_2 r^4 + k_3 r^6)$$
Explain barrel ($k_1 > 0$) vs pincushion ($k_1 < 0$) distortion geometrically.

3. **Tangential distortion** — Derive the full model (lens not parallel to sensor plane):
$$x_d \mathrel{+}= 2p_1 x_n y_n + p_2(r^2 + 2x_n^2)$$
$$y_d \mathrel{+}= p_1(r^2 + 2y_n^2) + 2p_2 x_n y_n$$

4. **Final pixel equation** — Write the complete distortion-aware projection:
$$u = f_x \cdot x_d + c_x, \quad v = f_y \cdot y_d + c_y$$

5. **Distortion Jacobian** — Derive the $2\times 2$ Jacobian $\frac{\partial \mathbf{x}_d}{\partial \mathbf{x}_n}$ for the combined radial + tangential model. This requires differentiating through $r^2 = x_n^2 + y_n^2$.

6. **Full projection Jacobian via chain rule** — Assemble the $2\times 3$ Jacobian of pixel coordinates w.r.t. 3D camera-frame point:
$$\frac{\partial \mathbf{p}}{\partial \mathbf{P}_c} = \underbrace{\frac{\partial \mathbf{p}}{\partial \mathbf{x}_d}}_{2\times2} \cdot \underbrace{\frac{\partial \mathbf{x}_d}{\partial \mathbf{x}_n}}_{2\times2} \cdot \underbrace{\frac{\partial \mathbf{x}_n}{\partial \mathbf{P}_c}}_{2\times3}$$
Derive each factor explicitly.

7. **Undistortion** — Explain why inverting distortion has no closed form. Describe the iterative Newton solve that OpenCV uses in `undistortPoints`. Write one Newton iteration step explicitly.

---

---

## M6 — Kalman Filter vs Nonlinear Optimization: Formalization & Differences

### Problem Statement
Both the Kalman Filter (KF) and nonlinear optimization (e.g. factor graph / bundle adjustment) solve estimation problems. Formalize both from a common probabilistic root — MAP estimation — and derive where and why they diverge.

### Setup
Given states $\mathbf{x}_{0:k}$ and measurements $\mathbf{z}_{1:k}$, the goal is:
$$\hat{\mathbf{x}} = \arg\max_{\mathbf{x}} p(\mathbf{x}_{0:k} \mid \mathbf{z}_{1:k})$$

### Tasks

1. **Common root: MAP estimation** — Apply Bayes' rule and take the negative log to convert MAP into a least-squares problem:
$$\hat{\mathbf{x}} = \arg\min_{\mathbf{x}} \left[ \sum_k \| \mathbf{z}_k - h(\mathbf{x}_k) \|^2_{\Sigma_R} + \sum_k \| \mathbf{x}_k - f(\mathbf{x}_{k-1}) \|^2_{\Sigma_Q} \right]$$
Show that under Gaussian noise this is exact, and write it as a factor graph.

2. **Kalman Filter as a special case** — Show that the KF solves the above incrementally and exactly when $f$ and $h$ are **linear**. Derive that the KF predict+update is equivalent to one step of Gaussian elimination on the information matrix. Specifically show:
   - Predict step → prior factor
   - Update step → measurement factor
   - KF is $O(n^2)$ per step in state dimension $n$

3. **Extended Kalman Filter (EKF)** — When $f$ and $h$ are nonlinear, the EKF linearizes them at the current estimate via first-order Taylor expansion. Derive the linearization:
$$f(\mathbf{x}) \approx f(\hat{\mathbf{x}}) + F(\mathbf{x} - \hat{\mathbf{x}}), \quad F = \left.\frac{\partial f}{\partial \mathbf{x}}\right|_{\hat{\mathbf{x}}}$$
State why this is only a local approximation and when it fails (high nonlinearity, poor initialization).

4. **Nonlinear optimization (full batch)** — The full MAP problem keeps all states and measurements simultaneously and solves iteratively with Gauss-Newton or Levenberg-Marquardt:
$$\delta\mathbf{x} = -(J^\top \Sigma^{-1} J)^{-1} J^\top \Sigma^{-1} \mathbf{r}$$
where $J$ is the full Jacobian over all states and $\mathbf{r}$ is the full residual vector. Explain why the Hessian $H = J^\top \Sigma^{-1} J$ is **sparse** (factor graph structure) and how this is exploited.

5. **Key differences — formalize** — Fill in this comparison from first principles:

| Property | Kalman Filter (EKF) | Nonlinear Optimization |
|----------|--------------------|-----------------------|
| State history | Marginalize past states | Keep all states |
| Linearization point | Once, at current estimate | Re-linearized each iteration |
| Iterations | Single pass (no re-linearization) | Multiple Gauss-Newton steps |
| Accuracy under nonlinearity | Degrades (linearization error) | Higher (iterative correction) |
| Computational cost | $O(n^2)$ per step | $O(N \cdot n^2)$, $N$ = window size |
| Consistency | Can become inconsistent (FEJ fixes this) | Consistent if converged |
| Real-time suitability | Yes | Sliding window only |

Derive each row from the math, not just state it.

6. **Sliding Window Optimization** — Explain how modern VIO systems (e.g. VINS-Mono, OKVIS) bridge the gap: keep a fixed window of states for batch optimization, marginalize older states into a prior using the Schur complement. Derive the marginalization step:
$$\Sigma_{\text{prior}} = \Sigma_{AA} - \Sigma_{AB}\Sigma_{BB}^{-1}\Sigma_{BA}$$
where $A$ = states to keep, $B$ = states to marginalize.

7. **When to use which** — Based on your derivations, state the conditions under which each is preferred:
   - Highly nonlinear system → ?
   - Strict real-time, low compute → ?
   - Post-processing / loop closure → ?
   - Long-horizon consistency matters → ?

---

---

## M7 — Gauss-Newton vs Levenberg-Marquardt vs Dogleg

### Problem Statement
All three algorithms minimize a nonlinear least-squares objective $F(\mathbf{x}) = \frac{1}{2}\|\mathbf{r}(\mathbf{x})\|^2$. Derive each method from first principles, identify what problem each solves that the previous one doesn't, and formalize the trade-offs.

### Setup
Residual vector $\mathbf{r}(\mathbf{x}) \in \mathbb{R}^m$, state $\mathbf{x} \in \mathbb{R}^n$, Jacobian $J = \frac{\partial \mathbf{r}}{\partial \mathbf{x}} \in \mathbb{R}^{m \times n}$.  
Approximate Hessian: $H \approx J^\top J$ (ignoring second-order terms).  
Gradient: $\mathbf{g} = J^\top \mathbf{r}$.

---

### Part 1: Gauss-Newton

1. **Derivation** — Linearize $\mathbf{r}(\mathbf{x} + \delta\mathbf{x}) \approx \mathbf{r}(\mathbf{x}) + J\delta\mathbf{x}$ and minimize over $\delta\mathbf{x}$:
$$\min_{\delta\mathbf{x}} \frac{1}{2}\|J\delta\mathbf{x} + \mathbf{r}\|^2$$
Show the solution is the **normal equations**:
$$J^\top J \, \delta\mathbf{x} = -J^\top \mathbf{r} \implies \delta\mathbf{x}_{GN} = -(J^\top J)^{-1} J^\top \mathbf{r}$$

2. **Geometric interpretation** — $\delta\mathbf{x}_{GN}$ is the least-squares solution to the linearized system. Draw the quadratic approximation bowl and the Newton step.

3. **Failure mode** — What happens when $J^\top J$ is singular or near-singular (rank-deficient, degenerate geometry)? What happens far from the minimum where the linearization is poor (diverges with large steps)?

---

### Part 2: Levenberg-Marquardt

4. **Motivation** — LM damps the GN step to stay within a region where the linearization is trustworthy. Derive the LM step by adding a damping term $\lambda I$ to the Hessian:
$$(J^\top J + \lambda I)\, \delta\mathbf{x}_{LM} = -J^\top \mathbf{r}$$

5. **Interpolation between GN and gradient descent** — Show:
   - $\lambda \to 0$: $\delta\mathbf{x}_{LM} \to \delta\mathbf{x}_{GN}$ (Gauss-Newton)
   - $\lambda \to \infty$: $\delta\mathbf{x}_{LM} \to -\frac{1}{\lambda}\mathbf{g}$ (steepest descent, tiny step)

6. **Trust region interpretation** — Show that the LM step is the exact solution to:
$$\min_{\delta\mathbf{x}} \frac{1}{2}\|J\delta\mathbf{x} + \mathbf{r}\|^2 \quad \text{s.t.} \quad \|\delta\mathbf{x}\| \leq \Delta$$
for some radius $\Delta$ that depends on $\lambda$. Derive the relationship between $\lambda$ and $\Delta$.

7. **Adaptive $\lambda$ update** — Define the gain ratio:
$$\rho = \frac{F(\mathbf{x}) - F(\mathbf{x} + \delta\mathbf{x})}{L(\mathbf{0}) - L(\delta\mathbf{x})}$$
where $L$ is the linearized model. Derive the update rule:
   - $\rho > 0.75$: good step, decrease $\lambda$ (expand trust region)
   - $\rho < 0.25$: poor step, increase $\lambda$ (shrink trust region, reject step)
   - $\rho < 0$: reject step entirely

---

### Part 3: Dogleg

8. **Motivation** — LM implicitly controls trust region via $\lambda$ but doesn't give direct control over step size. Dogleg explicitly constructs the best step within a trust region $\|\delta\mathbf{x}\| \leq \Delta$ by combining two anchor steps.

9. **Two anchor steps** — Derive:
   - **Steepest descent step** (Cauchy point): the optimal step along $-\mathbf{g}$ within the trust region:
$$\delta\mathbf{x}_{SD} = -\frac{\|\mathbf{g}\|^2}{\|J\mathbf{g}\|^2}\mathbf{g}$$
   - **Gauss-Newton step**: $\delta\mathbf{x}_{GN} = -(J^\top J)^{-1}J^\top\mathbf{r}$

10. **Dogleg path** — The dogleg step interpolates along the path SD → GN as $\Delta$ increases:
$$\delta\mathbf{x}_{DL}(\Delta) = \begin{cases} \frac{\Delta}{\|\delta\mathbf{x}_{SD}\|}\delta\mathbf{x}_{SD} & \Delta \leq \|\delta\mathbf{x}_{SD}\| \\ \delta\mathbf{x}_{SD} + \beta(\delta\mathbf{x}_{GN} - \delta\mathbf{x}_{SD}) & \|\delta\mathbf{x}_{SD}\| < \Delta < \|\delta\mathbf{x}_{GN}\| \\ \delta\mathbf{x}_{GN} & \Delta \geq \|\delta\mathbf{x}_{GN}\| \end{cases}$$
Derive $\beta$ such that $\|\delta\mathbf{x}_{DL}\| = \Delta$ in the middle case (solve a quadratic in $\beta$).

11. **Geometric picture** — Sketch the trust region circle, the Cauchy point, the GN point, and the dogleg path connecting them. Explain why the dogleg path is always inside the trust region.

---

### Part 4: Formal Comparison

12. **Fill in and derive each entry:**

| Property | Gauss-Newton | Levenberg-Marquardt | Dogleg |
|----------|-------------|--------------------|----|
| Step type | Normal equations | Damped normal equations | Piecewise SD→GN |
| Trust region | None (unbounded) | Implicit via $\lambda$ | Explicit radius $\Delta$ |
| Near minimum | Quadratic convergence | Quadratic convergence | Quadratic convergence |
| Far from minimum | Can diverge | Safe (large $\lambda$) | Safe (Cauchy point) |
| Singular $J^\top J$ | Fails | Regularized by $\lambda I$ | Uses pseudoinverse |
| Cost per iteration | Solve $n\times n$ system | Solve $n\times n$ system | Two solves + interpolation |
| Used in | Early SLAM (GraphSLAM) | Ceres, g2o default | g2o, Powell's method |

13. **Practical note for robotics** — Explain why Ceres Solver uses LM by default, why g2o exposes both LM and Dogleg, and when you'd switch from one to the other in a VIO/SLAM backend.

---

## M8 — CLAHE: Contrast Limited Adaptive Histogram Equalization

### Problem Statement
CLAHE is a preprocessing step widely used in visual odometry and SLAM pipelines (e.g. ORB-SLAM, VINS-Mono) to improve feature detection under uneven lighting. Derive the full algorithm from global histogram equalization up to CLAHE, and formalize why it matters for VIO.

### Background: Global Histogram Equalization (HE)

1. **Histogram and CDF** — For a grayscale image with pixel intensities $I(x,y) \in [0, L-1]$ (typically $L=256$), define:
$$h(k) = \text{number of pixels with intensity } k$$
$$\text{CDF}(k) = \sum_{j=0}^{k} h(j)$$

2. **Equalization transform** — The equalized intensity is:
$$I'(x,y) = \text{round}\left(\frac{\text{CDF}(I(x,y)) - \text{CDF}_{\min}}{N - \text{CDF}_{\min}} \cdot (L-1)\right)$$
where $N$ = total pixels, $\text{CDF}_{\min}$ = smallest non-zero CDF value. Derive this from the goal of making the output histogram uniform.

3. **Failure mode** — Explain why global HE fails on images with regions of very different brightness (e.g. a dark room with a bright window). The global CDF is dominated by the most frequent intensities — local contrast in dark/bright regions is lost.

### CLAHE Algorithm

4. **Adaptive HE (AHE)** — Divide the image into non-overlapping tiles (e.g. $8\times8$ grid). Apply HE independently in each tile. Explain why this fixes the locality problem but introduces a new one: over-amplification of noise in near-uniform regions.

5. **Contrast limiting** — Before computing the CDF, clip the histogram at a threshold $\text{clipLimit}$:
$$h_{\text{clipped}}(k) = \min(h(k),\ \text{clipLimit})$$
The excess counts $\sum_k \max(0, h(k) - \text{clipLimit})$ are redistributed uniformly across all bins:
$$h_{\text{final}}(k) = h_{\text{clipped}}(k) + \frac{\text{excess}}{L}$$
Derive how this limits the slope of the CDF (= local contrast amplification gain) and show the max gain is bounded by $\text{clipLimit} / \text{mean bin count}$.

6. **Bilinear interpolation between tiles** — Applying different equalization maps in adjacent tiles causes blocky artifacts at tile boundaries. CLAHE fixes this by interpolating the transfer functions of the 4 surrounding tile centers:
$$I'(x,y) = (1-t)(1-s)\,T_{00} + t(1-s)\,T_{10} + (1-t)s\,T_{01} + ts\,T_{11}$$
where $(s, t)$ are the fractional positions within the tile, and $T_{ij}$ is the equalized value from tile $(i,j)$'s transfer function. Derive $(s,t)$ from the pixel position and tile grid.

7. **Complexity** — Analyze the time complexity of CLAHE vs global HE:
   - Global HE: $O(N)$ where $N$ = number of pixels
   - CLAHE: $O(N + T \cdot L)$ where $T$ = number of tiles, $L$ = intensity levels
   Show this is still $O(N)$ for fixed $T$ and $L$.

### Why CLAHE in VIO/SLAM

8. **Feature detection sensitivity** — Explain why feature detectors (Harris, FAST, ORB) rely on local intensity gradients. Poor contrast → weak gradients → fewer detected features → track loss. CLAHE increases gradient magnitude in locally dark/bright regions without blowing out well-lit areas.

9. **Photometric consistency** — In direct methods (DSO, LSD-SLAM) that use raw pixel intensities, explain why CLAHE can hurt (breaks brightness constancy assumption between frames) while in indirect methods (feature-based) it helps.

10. **Practical parameters** — Formalize the two tunable parameters and their effects:
    - `clipLimit`: higher → more contrast enhancement → more noise amplification
    - `tileGridSize` (e.g. $8\times8$): smaller tiles → more local adaptation → higher compute
    State typical values used in robotics pipelines and the sensitivity to each.

---

---

## M9 — GFTT vs FAST: Corner Detection Derivations & Comparison

### Problem Statement
Good Features To Track (GFTT/Shi-Tomasi) and FAST are the two most common feature detectors in VIO and visual SLAM pipelines. Derive both from first principles and formalize when to prefer each.

### Part 1: Harris Corner Detector (foundation for GFTT)

1. **Structure tensor** — Define the auto-correlation matrix (second moment matrix) for a patch around pixel $(x,y)$:
$$M = \sum_{(u,v) \in W} w(u,v) \begin{bmatrix} I_x^2 & I_x I_y \\ I_x I_y & I_y^2 \end{bmatrix}$$
where $I_x, I_y$ are image gradients, $w$ is a Gaussian window. Explain what $M$'s eigenvalues $\lambda_1, \lambda_2$ tell you about local image structure:
   - Both small → flat region
   - One large, one small → edge
   - Both large → corner

2. **Harris response** — To avoid computing eigenvalues explicitly, Harris defines:
$$R = \det(M) - k \cdot \text{tr}(M)^2 = \lambda_1\lambda_2 - k(\lambda_1+\lambda_2)^2$$
Derive this. Show $R > 0$ at corners, $R < 0$ at edges, $|R|$ small in flat regions. Typical $k \in [0.04, 0.06]$.

### Part 2: GFTT (Shi-Tomasi)

3. **Shi-Tomasi score** — Instead of the Harris $R$, Shi-Tomasi uses:
$$R_{ST} = \min(\lambda_1, \lambda_2)$$
Derive why this is a better cornerness measure: a point is a good feature to track if and only if **both** eigenvalues are large, so the minimum is the bottleneck. Show that Shi-Tomasi strictly dominates Harris for tracking stability.

4. **Optical flow connection** — Show that $\min(\lambda_1,\lambda_2)$ directly bounds the condition number of the system solved in Lucas-Kanade optical flow. A small $\lambda_{\min}$ means the flow equation is ill-conditioned → poor tracking. Derive this connection explicitly.

5. **Non-maximum suppression** — Describe the NMS step: retain a pixel only if its score is the maximum in a $r\times r$ neighborhood. Why is this needed? What does it control (feature density)?

### Part 3: FAST (Features from Accelerated Segment Test)

6. **Segment test** — FAST examines 16 pixels on a Bresenham circle of radius 3 around candidate $p$. A pixel is a corner if there exist $N$ contiguous pixels on the circle all brighter than $I_p + t$ or all darker than $I_p - t$ (typically $N=12$ for FAST-12, $t$ = threshold). Formalize this as:
$$\text{corner}(p) = \exists \text{ arc of length } N \text{ s.t. } \forall q \in \text{arc}: I_q > I_p + t \;\text{or}\; I_q < I_p - t$$

7. **Speed-up: 4-point pre-test** — Before checking all 16 pixels, FAST checks only pixels 1, 5, 9, 13. Show that at least 3 of these 4 must satisfy the threshold condition for a corner to exist. This rejects most non-corners with 4 comparisons.

8. **Machine-learned decision tree** — The original FAST paper trains a decision tree on the 16-pixel order to minimize the number of comparisons needed. Describe what is being learned and why it is faster than a fixed order.

9. **FAST score** — The detector response is the largest threshold $t$ for which $p$ is still detected as a corner. Use this for NMS to keep the strongest corners.

### Part 4: Formal Comparison

10. **Derive and fill in:**

| Property | GFTT (Shi-Tomasi) | FAST |
|----------|------------------|------|
| Cornerness measure | $\min(\lambda_1, \lambda_2)$ — eigenvalue of $M$ | Segment test on circle of 16 pixels |
| Computation | Gradient → $M$ → eigenvalues: $O(N \cdot W^2)$ | 4-point pretest → circle check: $O(N)$ |
| Rotation invariance | Yes (eigenvalues) | Yes (circular mask) |
| Scale invariance | No (fixed window $W$) | No (fixed radius 3) |
| Sub-pixel accuracy | Yes (fit quadratic to score) | No (integer pixel) |
| Tracking quality | High (directly optimizes Lucas-Kanade condition) | Medium |
| Speed | Slower (gradient + eigen-decomp) | Very fast (few comparisons) |
| Typical use | Sparse VIO front-ends (VINS, MSCKF) | Real-time on embedded (FAST+optical flow) |

11. **When to use which in robotics** — Formalize the decision:
   - Low-compute embedded (drone, MCU): FAST — justify from operation count
   - High-quality sparse tracking (MSCKF, VINS-Mono): GFTT — justify from tracking error bound
   - ORB-SLAM: FAST for detection, then ORB descriptor — explain why GFTT isn't used here
   - Direct methods (DSO): neither — explain why

---

---

## M10 — SuperPoint vs GFTT: Architecture, Training, and Why It's Better

### Problem Statement
SuperPoint (DeTone et al., 2018) is a self-supervised CNN-based detector+descriptor that replaces classical detectors like GFTT in learned SLAM/VIO systems. Understand the full model architecture, training methodology, and formalize why it outperforms GFTT.

---

### Part 1: GFTT Limitations (recap)

1. **Failure modes** — Formalize exactly where GFTT breaks:
   - Viewpoint change: $M$ is computed in a fixed local patch — large rotation/scale changes the eigenvalue structure
   - Illumination change: gradient magnitudes scale with lighting — threshold $t$ on $\lambda_{\min}$ is not photometrically invariant
   - Textureless/repetitive regions: many spurious corners with similar scores → poor NMS
   - Descriptor: GFTT only detects — it needs a separate descriptor (BRIEF, ORB) which is hand-crafted and brittle

---

### Part 2: SuperPoint Architecture

2. **Shared encoder** — SuperPoint uses a VGG-style encoder that processes the full image once and produces a shared feature map. Describe the exact layer structure:

```
Input: H×W×1 (grayscale)
  Conv 3×3, 64, ReLU
  Conv 3×3, 64, ReLU
  MaxPool 2×2  → H/2 × W/2
  Conv 3×3, 64, ReLU
  Conv 3×3, 64, ReLU
  MaxPool 2×2  → H/4 × W/4
  Conv 3×3, 128, ReLU
  Conv 3×3, 128, ReLU
  MaxPool 2×2  → H/8 × W/8
  Conv 3×3, 128, ReLU
  Conv 3×3, 128, ReLU
         → shared feature map: H/8 × W/8 × 128
```

Explain why all convolutions are $3\times3$ (receptive field growth, parameter efficiency) and why 3 max-pool layers give stride 8 overall.

3. **Detector head** — Takes the $H/8 \times W/8 \times 128$ feature map and produces a keypoint heatmap:
```
Conv 3×3, 256, ReLU
Conv 1×1, 65         → H/8 × W/8 × 65
Reshape + Softmax    → H × W × 1  (keypoint probability map)
```
Explain the 65-channel output: 64 = $8\times8$ pixel cells + 1 "no keypoint" dustbin channel. Derive how the $H/8 \times W/8 \times 65$ tensor is reshaped to a full-resolution $H \times W$ heatmap via pixel shuffle / depth-to-space.

4. **Descriptor head** — Takes the same feature map and produces dense descriptors:
```
Conv 3×3, 256, ReLU
Conv 1×1, 256        → H/8 × W/8 × 256
Bicubic upsample     → H × W × 256
L2-normalize per pixel
```
Explain why descriptors are L2-normalized (unit sphere → cosine similarity = dot product). Why is the descriptor head separate from the detector head?

---

### Part 3: Training Methodology

5. **Stage 1 — MagicPoint on synthetic data** — SuperPoint is first trained on synthetic geometric shapes (lines, triangles, quadrilaterals, ellipses) where ground-truth keypoint locations are known exactly (corners of shapes). The detector head is trained with a cross-entropy loss over the 65-channel output:
$$\mathcal{L}_{det} = -\sum_{(i,j)} \log p_{ij}(c_{ij})$$
where $c_{ij} \in \{0,\ldots,63,\text{dustbin}\}$ is the ground-truth cell label. Explain why synthetic data is sufficient for this stage.

6. **Stage 2 — Homographic Adaptation (self-supervised)** — To adapt to real images without manual labels, SuperPoint applies random homographies $\mathcal{H}$ to real images and aggregates MagicPoint detections across many warped views:
$$\hat{D}(I) = \text{Aggregate}\left\{ \mathcal{H}^{-1} \cdot D(\mathcal{H}(I)) \mid \mathcal{H} \sim p(\mathcal{H}) \right\}$$
The aggregated pseudo-label $\hat{D}(I)$ is used to fine-tune the detector on real images. Explain:
   - Why a single forward pass on the original image is insufficient (domain gap)
   - Why aggregating over many homographies gives stable, consistent labels
   - What distribution $p(\mathcal{H})$ should cover (random rotation, scale, perspective)

7. **Descriptor training loss** — The descriptor head is trained jointly using a contrastive loss. Given a homography pair $(I_A, I_B = \mathcal{H}(I_A))$ and corresponding descriptor maps $\mathcal{D}_A$, $\mathcal{D}_B$:
$$\mathcal{L}_{desc} = \frac{1}{|S|}\sum_{(i,j)\in S} \left[ \lambda_d \cdot \ell_p(i,j) + (1-\lambda_d) \cdot \ell_n(i,j) \right]$$
where $\ell_p$ is the positive loss (matched pairs should have similar descriptors) and $\ell_n$ is the negative loss (non-matched pairs should differ). Derive both terms using hinge loss:
$$\ell_p = \max(0,\, m_p - \mathbf{d}_A(i)^\top \mathbf{d}_B(\hat{j}))$$
$$\ell_n = \max(0,\, \mathbf{d}_A(i)^\top \mathbf{d}_B(j') - m_n)$$
where $m_p, m_n$ are margins and $\hat{j}$ is the warped correspondence of $i$ under $\mathcal{H}$.

8. **Joint training** — The final model is trained end-to-end with:
$$\mathcal{L} = \mathcal{L}_{det} + \lambda \mathcal{L}_{desc}$$
State typical hyperparameters: image size $240\times320$, $\lambda=250$, margins $m_p=1, m_n=0.2$, batch size 32, Adam optimizer.

---

### Part 4: Why SuperPoint is Better Than GFTT

9. **Repeatability under viewpoint change** — GFTT detects corners of the eigenvalue structure which changes with viewpoint. SuperPoint learns a viewpoint-invariant detector via homographic adaptation. Formalize repeatability:
$$\text{Rep} = \frac{|\text{det}(I_A) \cap \mathcal{H}(\text{det}(I_B))|_{\epsilon}}{\min(|\text{det}(I_A)|, |\text{det}(I_B)|)}$$
where $|\cdot|_\epsilon$ counts correspondences within $\epsilon$ pixels. SuperPoint achieves higher Rep on HPatches benchmark — know the numbers (~0.65 vs ~0.53 for GFTT+BRIEF).

10. **Joint detection + description** — GFTT requires a separate descriptor (ORB, BRIEF) with no coupling between detection and description quality. SuperPoint trains both jointly — keypoints are detected where descriptors are most discriminative.

11. **Dense descriptors** — SuperPoint produces a $256$-dim descriptor at every pixel (not just keypoints). This enables:
    - Flexible matching (match any subset of keypoints post-hoc)
    - Integration with learned matchers (SuperGlue uses SuperPoint descriptors directly)

12. **Formal comparison table:**

| Property | GFTT | SuperPoint |
|----------|------|------------|
| Detection principle | $\min(\lambda_1,\lambda_2)$ of structure tensor | Learned CNN heatmap |
| Descriptor | Separate (ORB/BRIEF, hand-crafted) | Jointly trained 256-dim dense CNN |
| Viewpoint invariance | Low (gradient-based, patch-local) | High (homographic adaptation) |
| Illumination invariance | Low (gradient magnitudes scale) | Higher (learned from real images) |
| Repeatability (HPatches) | ~0.53 | ~0.65 |
| Runtime (GPU) | Fast (CPU only, $O(N)$) | ~12ms on GPU (full image) |
| Runtime (CPU) | Very fast | Slow (not practical without GPU) |
| Requires GPU | No | Yes (for real-time) |
| Training data needed | None | Synthetic shapes + real images |
| Used in | VINS-Mono, MSCKF | SuperGlue, NeRF-SLAM, learned VIO |

13. **When NOT to use SuperPoint** — Justify:
    - Embedded systems without GPU → FAST/GFTT win on latency
    - Dynamic lighting where training distribution is mismatched → classical more robust
    - Ultra-low-memory systems → 256-dim descriptors at full resolution are memory-intensive

---

## M11 — Descriptors, Classical vs Learned Matching, LightGlue

### Problem Statement
A descriptor encodes the local appearance around a keypoint into a vector so that corresponding points across images can be matched. Understand classical descriptors from first principles, their failure modes, and how LightGlue (Lindenberger et al., 2023) replaces the matching step entirely with a learned transformer.

---

### Part 1: What is a Descriptor?

1. **Formal definition** — A descriptor is a function $\phi: \mathbb{R}^{H\times W} \times \mathbb{R}^2 \to \mathbb{R}^d$ that maps an image and a keypoint location to a $d$-dimensional vector. Two keypoints $p_A, p_B$ in images $I_A, I_B$ are matched if:
$$\|\phi(I_A, p_A) - \phi(I_B, p_B)\|_2 < \tau$$
or equivalently their cosine similarity exceeds a threshold. Formalize what properties $\phi$ must have: repeatability, distinctiveness, invariance.

2. **The matching problem** — Given sets $\mathcal{A} = \{p_1,\ldots,p_M\}$ and $\mathcal{B} = \{q_1,\ldots,q_N\}$ with descriptors, matching is finding an assignment $\sigma: \mathcal{A} \to \mathcal{B} \cup \{\emptyset\}$ (partial assignment, some points may be unmatched). Naive approach: compute all $M \times N$ pairwise distances → $O(MNd)$. State why this is the bottleneck for large $M, N$.

---

### Part 2: Classical Descriptors

3. **SIFT (Scale-Invariant Feature Transform)** — Derive the descriptor construction:
   - Compute image gradients in a $16\times16$ patch around the keypoint
   - Divide into $4\times4$ sub-cells, each contributing an 8-bin gradient orientation histogram
   - Concatenate → $4\times4\times8 = 128$-dim vector
   - Normalize to unit length, clamp values $>0.2$, renormalize (removes illumination scale)
   - Rotate patch by dominant orientation for rotation invariance
   State: 128-dim float, ~512 bytes per keypoint, invariant to scale+rotation+illumination.

4. **BRIEF (Binary Robust Independent Elementary Features)** — BRIEF avoids floating-point descriptors. Derive:
   - Sample $n$ pairs of points $(p_i, q_i)$ in a patch using a fixed random pattern (Gaussian-distributed)
   - Each bit: $b_i = \mathbb{1}[I(p_i) > I(q_i)]$
   - Concatenate → $n$-bit binary string (typically $n=256$ or $512$)
   - Matching via Hamming distance: XOR + popcount → very fast on modern CPUs
   State: 32 bytes for 256-bit descriptor, not rotation-invariant.

5. **ORB (Oriented FAST + Rotated BRIEF)** — ORB adds rotation invariance to BRIEF:
   - Detect with FAST, compute orientation via intensity centroid moment:
$$\theta = \text{atan2}\left(\sum_{x,y} y \cdot I(x,y),\ \sum_{x,y} x \cdot I(x,y)\right)$$
   - Rotate the BRIEF sampling pattern by $\theta$ → steered BRIEF (rBRIEF)
   - 256-bit descriptor, rotation-invariant, fast. State: used in ORB-SLAM2/3.

6. **Failure modes of all classical descriptors:**
   - Large viewpoint change (>30°): patch appearance changes too much for any fixed transform
   - Low texture / repetitive patterns: many similar descriptors → ambiguous matches
   - Motion blur, HDR lighting: gradient structure corrupted
   - All use nearest-neighbour matching independently per keypoint — no global geometric consistency

---

### Part 3: SuperGlue (predecessor context)

7. **SuperGlue (Sarlin et al., 2020)** — The first learned matcher using a Graph Neural Network + Sinkhorn optimal transport. Briefly formalize:
   - Augment descriptors with keypoint positions via MLP encoder
   - Run $L=9$ layers of self+cross attention between $\mathcal{A}$ and $\mathcal{B}$
   - Produce a score matrix $S \in \mathbb{R}^{(M+1)\times(N+1)}$ (dustbin row/col for unmatched)
   - Solve assignment via Sinkhorn iterations (differentiable optimal transport)
   - State the problem: runs all $L$ layers on all keypoints regardless of difficulty → slow, $O(MN \cdot L)$ attention

---

### Part 4: LightGlue Architecture

8. **Key insight** — Not all image pairs are equally hard to match. Easy pairs (small viewpoint change) need fewer layers; hard pairs need more. LightGlue introduces **adaptive depth** — it stops processing early when confident enough.

9. **Input encoding** — Each keypoint $i$ in image $A$ is encoded as:
$$\mathbf{f}_i^{(0)} = \text{MLP}(\mathbf{d}_i) + \text{PE}(p_i)$$
where $\mathbf{d}_i \in \mathbb{R}^{256}$ is the SuperPoint descriptor and $\text{PE}(p_i)$ is a learned rotary positional encoding of the 2D keypoint location. Explain why adding position to the descriptor (not concatenating) is better — avoids the network ignoring one modality.

10. **Transformer layers** — LightGlue stacks $L=9$ identical layers. Each layer has:
    - **Self-attention** within each image (keypoints attend to each other):
$$\mathbf{f}_i \leftarrow \mathbf{f}_i + \text{Attention}(\mathbf{f}_i,\ \mathbf{F}_A)$$
    - **Cross-attention** between images (keypoints attend to the other image):
$$\mathbf{f}_i \leftarrow \mathbf{f}_i + \text{Attention}(\mathbf{f}_i,\ \mathbf{F}_B)$$
    Each attention uses multi-head attention with $h=4$ heads, $d_{head}=64$. Total model: $\approx 5.3$M parameters.

11. **Rotary positional encoding (RoPE)** — LightGlue uses RoPE to encode keypoint $(x,y)$ positions into the attention keys/queries directly, without adding a separate positional embedding. Derive: for a 2D position $p = (x,y)$, the rotation matrix $R_\theta$ rotates query/key vectors in 2D blocks:
$$\text{RoPE}(\mathbf{q}, p) = R_\theta(p) \mathbf{q}$$
This makes attention scores naturally depend on relative position: $\mathbf{q}_i^\top \mathbf{k}_j \propto f(\|p_i - p_j\|)$.

12. **Assignment head** — After all transformer layers, LightGlue computes a soft assignment matrix:
$$S_{ij} = \mathbf{f}_i^{A\top} \mathbf{f}_j^B \quad \in \mathbb{R}^{M\times N}$$
Then solves optimal transport via log-space Sinkhorn iterations to get a doubly-stochastic assignment $P$ with dustbin. Final matches: $\sigma(i) = \arg\max_j P_{ij}$ if $\max_j P_{ij} > \tau$.

13. **Adaptive depth (early exit)** — At each layer $\ell$, LightGlue predicts a per-keypoint confidence $c_i^\ell \in [0,1]$. If $c_i^\ell > \theta_{exit}$, keypoint $i$ is removed from further computation. This gives:
    - Easy image pairs: exit after $\sim3$ layers → 3× speedup
    - Hard pairs: all 9 layers used
    Derive that the expected compute is $O(L_{eff} \cdot M \cdot N)$ where $L_{eff} < L$ adapts to difficulty.

14. **Training** — LightGlue is trained on MegaDepth (internet photos with SfM ground truth) and ScanNet (indoor RGB-D). Loss is negative log-likelihood of correct assignment under the Sinkhorn distribution:
$$\mathcal{L} = -\frac{1}{|\mathcal{M}|}\sum_{(i,j)\in\mathcal{M}} \log P_{ij} - \frac{1}{|\mathcal{U}_A|}\sum_{i\in\mathcal{U}_A}\log P_{i,\emptyset} - \frac{1}{|\mathcal{U}_B|}\sum_{j\in\mathcal{U}_B}\log P_{\emptyset,j}$$
where $\mathcal{M}$ = ground-truth matched pairs, $\mathcal{U}_A, \mathcal{U}_B$ = unmatched keypoints. Train with Adam, learning rate $10^{-4}$, batch size 32, 200K iterations.

---

### Part 5: Why LightGlue Beats Classical Descriptors

15. **Global geometric consistency** — Classical matching is per-keypoint: each match is independent. LightGlue's self-attention lets keypoints reason about each other → a keypoint near many other matched keypoints is more likely to be correct. Formalize as: LightGlue implicitly enforces epipolar/homography consistency without RANSAC.

16. **Handling ambiguity** — In repetitive scenes (brick walls, corridors), many SIFT/ORB descriptors are similar → many false matches. LightGlue uses cross-attention to disambiguate: it can ask "given what I see in the other image globally, which of these similar descriptors is the right match?"

17. **Soft unmatched handling** — Classical methods use ratio test (Lowe's ratio): reject if $d_1/d_2 > 0.8$. This is a hard heuristic. LightGlue's dustbin learns a soft, data-driven threshold for unmatchability.

18. **Formal comparison:**

| Property | SIFT+NN | ORB+Hamming | SuperPoint+SuperGlue | SuperPoint+LightGlue |
|----------|---------|-------------|---------------------|---------------------|
| Descriptor dim | 128 float | 256 bit | 256 float | 256 float |
| Matching strategy | Nearest neighbour | Nearest neighbour (Hamming) | GNN + Sinkhorn | Transformer + adaptive Sinkhorn |
| Geometric consistency | None (need RANSAC) | None (need RANSAC) | Implicit (GNN) | Implicit (attention) |
| Unmatched handling | Ratio test | Ratio test | Dustbin | Learned dustbin |
| Speed (GPU, 512 kpts) | Fast | Very fast | ~70ms | ~25ms |
| Adaptive compute | No | No | No | Yes (early exit) |
| AUC@5° (MegaDepth) | ~47 | ~32 | ~73 | ~76 |
| Parameters | 0 | 0 | 12M | 5.3M |

19. **When LightGlue falls short** — Formalize:
    - No GPU: transformer attention is $O(M^2)$ in memory on CPU → impractical
    - Extreme domain shift (underwater, thermal, night) without fine-tuning
    - Very sparse keypoints ($M < 50$): classical NN matching is faster and sufficient
    - Need interpretable matches (safety-critical systems): black-box attention is harder to audit

---

## M12 — Lucas-Kanade Optical Flow: Derivation, Pyramids, vs Descriptor Matching

### Problem Statement
Lucas-Kanade (LK) optical flow is the dominant method for tracking keypoints frame-to-frame in VIO systems (VINS-Mono, MSCKF, OpenVIO). Derive it from the brightness constancy assumption, understand its failure modes, and formalize how it differs from descriptor matching.

---

### Part 1: How Descriptors Are Matched (recap)

1. **Descriptor matching pipeline** — Formalize the three steps:
   - **Extract**: compute $\phi(I_A, p_i) \in \mathbb{R}^d$ for each keypoint $p_i \in \mathcal{A}$ in image $A$
   - **Detect in B**: run detector on $I_B$ independently → keypoints $\mathcal{B}$, descriptors $\phi(I_B, q_j)$
   - **Match**: find assignment $\sigma$ minimizing descriptor distance, e.g. nearest neighbour:
$$\sigma(i) = \arg\min_j \|\phi(I_A, p_i) - \phi(I_B, q_j)\|_2$$
   Optionally filter with Lowe's ratio test: $d_1/d_2 < 0.8$.

2. **Key property** — Descriptor matching is **detection-then-match**: both images are processed independently. It finds correspondences even between non-consecutive frames (wide baseline). Cost: $O(MNd)$ for brute force, $O(Md\log N)$ with FLANN/KD-tree.

---

### Part 2: Lucas-Kanade Optical Flow — Derivation

3. **Brightness constancy assumption** — The core assumption: the intensity of a pixel does not change as it moves between frames:
$$I(x, y, t) = I(x + u, y + v, t + 1)$$
where $(u, v)$ is the unknown flow (displacement). This is only valid for small motions and Lambertian surfaces.

4. **Linearization** — Taylor-expand the right side around $(x, y, t)$:
$$I(x+u, y+v, t+1) \approx I(x,y,t) + I_x u + I_y v + I_t$$
Substituting into the brightness constancy equation:
$$I_x u + I_y v + I_t = 0$$
This is the **optical flow constraint equation** — one equation, two unknowns $(u,v)$. Underdetermined — the aperture problem.

5. **Lucas-Kanade: solve over a patch** — Assume flow $(u,v)$ is constant within a $W\times W$ window around the keypoint. Stack the constraint for all $n = W^2$ pixels:
$$\underbrace{\begin{bmatrix} I_x(p_1) & I_y(p_1) \\ \vdots & \vdots \\ I_x(p_n) & I_y(p_n) \end{bmatrix}}_{A \in \mathbb{R}^{n\times2}} \begin{bmatrix} u \\ v \end{bmatrix} = -\underbrace{\begin{bmatrix} I_t(p_1) \\ \vdots \\ I_t(p_n) \end{bmatrix}}_{\mathbf{b} \in \mathbb{R}^n}$$
Solve via least squares (normal equations):
$$(A^\top A)\begin{bmatrix}u\\v\end{bmatrix} = -A^\top \mathbf{b}$$
Note that $A^\top A = M$ is exactly the **structure tensor** from GFTT. Derive the connection: LK is well-conditioned iff $\lambda_{\min}(M)$ is large — this is why GFTT features are good to track.

6. **Iterative refinement** — Because the Taylor expansion is only valid for small $(u,v)$, LK is iterated: warp the patch by the current estimate, recompute residuals, solve for a correction $(\Delta u, \Delta v)$, repeat until convergence. Formalize the iteration:
$$\begin{bmatrix}u^{k+1}\\v^{k+1}\end{bmatrix} = \begin{bmatrix}u^k\\v^k\end{bmatrix} + (A^\top A)^{-1} A^\top \mathbf{b}^k$$
where $\mathbf{b}^k$ uses the current warped image.

---

### Part 3: Pyramidal LK (Bouguet's Method)

7. **Problem with large motion** — LK assumes small motion (Taylor linearization valid). For fast-moving cameras or large inter-frame motion, $(u,v)$ can be many pixels — linearization fails.

8. **Image pyramid** — Build a $L$-level Gaussian pyramid $\{I^0, I^1, \ldots, I^{L-1}\}$ where $I^\ell$ is $I^{\ell-1}$ downsampled by 2. At level $\ell$, a displacement of 1 pixel corresponds to $2^\ell$ pixels in the original image. Typical: $L=4$ levels.

9. **Coarse-to-fine tracking** — Start at the coarsest level $L-1$ with initial guess $(u,v)=(0,0)$, run LK to convergence, propagate the estimate to the next finer level, repeat:
$$g^\ell = 2(g^{\ell+1} + d^{\ell+1})$$
where $g^\ell$ is the initial guess at level $\ell$ and $d^{\ell+1}$ is the flow estimated at level $\ell+1$. At level 0, the final flow is $g^0 + d^0$. Derive the effective displacement range: with $L=4$ levels and max 10-pixel search per level → up to $10 \times 2^3 = 80$ pixel displacement at full resolution.

10. **Complexity** — Each LK solve at level $\ell$ is $O(W^2)$ to build $A^\top A$ and $O(1)$ to solve (2×2 system). Total per keypoint: $O(L \cdot W^2 \cdot K)$ where $K$ = number of iterations. Compare to descriptor matching: $O(d)$ per pair but requires detection in both frames.

---

### Part 4: LK vs Descriptor Matching — Formal Comparison

11. **Derive and fill in:**

| Property | LK Optical Flow | Descriptor Matching |
|----------|----------------|---------------------|
| Requires detection in frame B | No — tracks from A to B directly | Yes — detect independently in B |
| Motion assumption | Small motion (patch constant) | None (can match across wide baseline) |
| Uses appearance model | Patch intensity directly | Encoded descriptor $\phi \in \mathbb{R}^d$ |
| Accuracy | Sub-pixel (iterative refinement) | Pixel-level (integer keypoint locations) |
| Handles large motion | Yes (with pyramid) up to ~80px | Yes (descriptor invariant) |
| Handles illumination change | Poor (brightness constancy) | Better (SIFT normalized, SuperPoint learned) |
| Handles viewpoint change | Poor (>15° degrades) | Better (SIFT rotation/scale invariant) |
| Computational cost | $O(L \cdot W^2 \cdot K)$ per point | $O(MNd)$ brute force |
| Consistency check | Forward-backward error check | Ratio test / mutual nearest neighbour |
| Typical use | Consecutive frame tracking (VIO) | Loop closure, wide-baseline matching |
| Examples in SLAM | VINS-Mono, MSCKF, SVO | ORB-SLAM (relocalization), SuperGlue |

12. **Forward-backward consistency check** — LK tracks $p_A \to \hat{p}_B$ in the forward direction, then tracks $\hat{p}_B \to \hat{p}_A$ backward. If $\|p_A - \hat{p}_A\| > \epsilon$ (typically 1px), the track is rejected. Derive why this catches failures: if LK converges to a wrong local minimum in either direction, the backward track will disagree.

13. **When to use which in a VIO pipeline** — Formalize:
    - Front-end frame-to-frame tracking (high frame rate, small motion): **LK** — no redundant detection, sub-pixel, fast
    - Loop closure (large temporal gap, wide baseline): **Descriptor matching** — LK cannot bridge large displacements
    - Relocalization after tracking failure: **Descriptor matching** — need to re-establish correspondence from scratch
    - Map point projection tracking (known 3D → 2D): **LK** initialized at projected point → very efficient

---

## M13 — Essential Matrix, Fundamental Matrix, and Depth from Matches

### Problem Statement
Given point correspondences across two views, the Essential and Fundamental matrices encode the geometric constraint between them. Derive both, understand what depth looks like geometrically, and formalize how depth is recovered from matched points.

### Part 1: Homogeneous Coordinates (foundation)

1. **Why homogeneous coordinates** — A 2D point $(u,v)$ is represented as $\tilde{\mathbf{p}} = [u, v, 1]^\top$. Projection $\mathbf{p} = K\mathbf{P}_c$ is linear in homogeneous form but nonlinear in Cartesian (division by $Z$). Show that two homogeneous vectors $\lambda\tilde{\mathbf{p}}$ and $\tilde{\mathbf{p}}$ represent the same 2D point for any $\lambda \neq 0$ — they live on the same ray through the origin in $\mathbb{R}^3$.

2. **Cross product as a line** — A 2D line $\ell$ passing through two homogeneous points $\tilde{\mathbf{p}}_1, \tilde{\mathbf{p}}_2$ is $\ell = \tilde{\mathbf{p}}_1 \times \tilde{\mathbf{p}}_2$. A point lies on line $\ell$ iff $\tilde{\mathbf{p}}^\top \ell = 0$. Derive both. This is used in the epipolar constraint below.

### Part 2: Essential Matrix

3. **Setup** — Two calibrated cameras (known $K_A, K_B$). A 3D point $\mathbf{P}$ projects to normalized image coordinates $\mathbf{x}_A = K_A^{-1}\tilde{\mathbf{p}}_A$ and $\mathbf{x}_B = K_B^{-1}\tilde{\mathbf{p}}_B$ (backprojected rays). The relative pose between cameras is $(R, \mathbf{t})$ with $\mathbf{P}_B = R\mathbf{P}_A + \mathbf{t}$.

4. **Coplanarity constraint** — The vectors $\mathbf{x}_A$, $\mathbf{x}_B$, and $\mathbf{t}$ are coplanar (they all point to the same 3D point). Derive the scalar triple product:
$$\mathbf{x}_B^\top (\mathbf{t} \times R\mathbf{x}_A) = 0$$
Rewrite using the skew-symmetric matrix $[\mathbf{t}]_\times$ (where $\mathbf{t}\times\mathbf{v} = [\mathbf{t}]_\times \mathbf{v}$):
$$\mathbf{x}_B^\top [\mathbf{t}]_\times R\, \mathbf{x}_A = 0$$
Define the **Essential matrix**: $E = [\mathbf{t}]_\times R \in \mathbb{R}^{3\times3}$. The epipolar constraint is:
$$\mathbf{x}_B^\top E\, \mathbf{x}_A = 0$$

5. **Properties of E** — Show:
   - $\text{rank}(E) = 2$ (singular — one zero singular value)
   - $E = U\,\text{diag}(1,1,0)\,V^\top$ in SVD
   - Given $E$, recover $(R,\mathbf{t})$ with 4 candidate solutions, disambiguate by cheirality (positive depth)

### Part 3: Fundamental Matrix

6. **Uncalibrated case** — For uncalibrated cameras, work in pixel coordinates $\tilde{\mathbf{p}}_A, \tilde{\mathbf{p}}_B$. Substitute $\mathbf{x} = K^{-1}\tilde{\mathbf{p}}$:
$$\tilde{\mathbf{p}}_B^\top \underbrace{K_B^{-\top} E K_A^{-1}}_{F} \tilde{\mathbf{p}}_A = 0$$
Define the **Fundamental matrix**: $F = K_B^{-\top} E K_A^{-1} \in \mathbb{R}^{3\times3}$. The constraint is:
$$\tilde{\mathbf{p}}_B^\top F\, \tilde{\mathbf{p}}_A = 0$$

7. **Epipolar line** — For a point $\tilde{\mathbf{p}}_A$ in image $A$, the corresponding epipolar line in image $B$ is $\ell_B = F\tilde{\mathbf{p}}_A$. The matching point $\tilde{\mathbf{p}}_B$ must lie on $\ell_B$. Derive this reduces search from 2D to 1D — fundamental for efficient matching and outlier rejection.

8. **E vs F comparison:**

| | Essential Matrix $E$ | Fundamental Matrix $F$ |
|--|---------------------|----------------------|
| Works with | Normalized/calibrated coords | Pixel coords |
| Encodes | Relative pose $(R,\mathbf{t})$ | Relative pose + intrinsics |
| DOF | 5 (up to scale) | 7 (up to scale) |
| Min correspondences | 5-point algorithm | 7- or 8-point algorithm |
| Rank | 2 | 2 |
| Use in SLAM | Initialization (calibrated) | Uncalibrated SfM |

### Part 4: Depth from Matches

9. **Triangulation** — Given matched normalized points $\mathbf{x}_A, \mathbf{x}_B$ and relative pose $(R,\mathbf{t})$, find 3D point $\mathbf{P}$. The two back-projected rays are:
$$\mathbf{P} = \lambda_A \mathbf{x}_A = R^\top(\lambda_B \mathbf{x}_B - \mathbf{t})$$
In practice rays don't intersect due to noise — solve in least squares via DLT (see M14) or midpoint method. Derive the linear system for $[\lambda_A, \lambda_B]^\top$.

10. **Depth sign and cheirality** — A valid reconstruction requires $\lambda_A > 0$ and $\lambda_B > 0$ (point in front of both cameras). This disambiguates the 4 $(R,\mathbf{t})$ solutions from the Essential matrix decomposition. Derive: of the 4 solutions, only 1 places the point in front of both cameras.

11. **What depth looks like geometrically** — Visualize: given a pixel $\tilde{\mathbf{p}}$ in image $A$, all possible 3D points that project to it form a **ray** from the camera center through $K_A^{-1}\tilde{\mathbf{p}}$. Depth is the scalar distance along this ray. The epipolar line in image $B$ is the projection of this ray onto $B$'s image plane. Matching constrains the 3D point to the intersection of two rays — triangulation.

---

## M14 — DLT: Direct Linear Transform

### Problem Statement
DLT is the foundational algorithm for solving homogeneous linear systems in geometry — used in triangulation, homography estimation, and camera calibration. Derive it from first principles using SVD.

### Part 1: Homogeneous Systems

1. **The problem** — Many geometric problems reduce to: given $n$ equations of the form $A\mathbf{x} = \mathbf{0}$ where $\mathbf{x}$ is the unknown (up to scale). The trivial solution $\mathbf{x}=\mathbf{0}$ is useless — we want the non-trivial solution on the unit sphere: $\min_{\mathbf{x}} \|A\mathbf{x}\|^2\ \text{s.t.}\ \|\mathbf{x}\|=1$.

2. **SVD solution** — Decompose $A = U\Sigma V^\top$. Show that:
$$\|A\mathbf{x}\|^2 = \mathbf{x}^\top V \Sigma^2 V^\top \mathbf{x}$$
The minimum under $\|\mathbf{x}\|=1$ is the **last column of $V$** (corresponding to the smallest singular value $\sigma_{\min}$). Derive this.

3. **Geometric interpretation** — The last column of $V$ is the direction in the null space of $A$ (or closest to it if $A$ is full rank due to noise). The smallest singular value measures how well-constrained the solution is: if $\sigma_{\min} \approx \sigma_{\min-1}$, the solution is degenerate.

### Part 2: DLT for Homography Estimation

4. **Homography** — A homography $H \in \mathbb{R}^{3\times3}$ maps $\tilde{\mathbf{p}}' = H\tilde{\mathbf{p}}$ (up to scale). Given correspondence $\tilde{\mathbf{p}}_i \leftrightarrow \tilde{\mathbf{p}}'_i$, the constraint $\tilde{\mathbf{p}}'_i \times H\tilde{\mathbf{p}}_i = \mathbf{0}$ gives 2 independent linear equations per correspondence in the 9 unknowns $\mathbf{h} = \text{vec}(H)$:
$$\begin{bmatrix} \mathbf{0}^\top & -w'_i\tilde{\mathbf{p}}_i^\top & y'_i\tilde{\mathbf{p}}_i^\top \\ w'_i\tilde{\mathbf{p}}_i^\top & \mathbf{0}^\top & -x'_i\tilde{\mathbf{p}}_i^\top \end{bmatrix} \mathbf{h} = \mathbf{0}$$
Stack $n \geq 4$ correspondences → $A\mathbf{h}=\mathbf{0}$, solve via SVD. Derive why 4 point pairs are the minimum (8 equations, 8 DOF for $H$ up to scale).

5. **Normalization** — DLT is sensitive to the scale of coordinates. Derive the normalization step (Hartley normalization): translate points to centroid, scale so RMS distance from origin is $\sqrt{2}$. Show this improves numerical conditioning of $A$.

### Part 3: DLT for Triangulation

6. **Triangulation via DLT** — Given cameras $P_A = K_A[I|\mathbf{0}]$ and $P_B = K_B[R|\mathbf{t}]$ and matched pixels $\tilde{\mathbf{p}}_A, \tilde{\mathbf{p}}_B$, the 3D point $\tilde{\mathbf{P}} = [X,Y,Z,1]^\top$ satisfies:
$$\tilde{\mathbf{p}}_A \times P_A\tilde{\mathbf{P}} = \mathbf{0}, \quad \tilde{\mathbf{p}}_B \times P_B\tilde{\mathbf{P}} = \mathbf{0}$$
Write these as $A\tilde{\mathbf{P}} = \mathbf{0}$ and solve via SVD. Each correspondence gives 2 equations → 4 equations total for the 4-vector $\tilde{\mathbf{P}}$. Derive the $4\times4$ matrix $A$ explicitly.

---

## M15 — PnP and Variants: P3P, EPnP, and Homogeneous Coordinates

### Problem Statement
Perspective-n-Point (PnP) estimates camera pose given $n$ 3D-2D point correspondences. It is the core of visual odometry re-localization, map point tracking, and SLAM front-ends. Derive the problem formulation and key variants.

### Part 1: Problem Formulation

1. **PnP setup** — Given $n$ 3D points $\{\mathbf{P}_i\}$ in world frame and their 2D projections $\{\tilde{\mathbf{p}}_i\}$ in the camera image (known correspondences), find $R, \mathbf{t}$ such that:
$$\lambda_i \tilde{\mathbf{p}}_i = K(R\mathbf{P}_i + \mathbf{t}), \quad \lambda_i = [R\mathbf{P}_i + \mathbf{t}]_z$$
This is a 6-DOF pose estimation problem (3 rotation + 3 translation).

2. **Minimum cases** — Derive the minimum number of correspondences:
   - $n=3$: P3P — up to 4 solutions (finite, need a 4th point to disambiguate)
   - $n=4$: unique solution in general position
   - $n\geq6$: overdetermined, solve with DLT or nonlinear optimization

### Part 2: DLT-PnP (n ≥ 6)

3. **Linear formulation** — Rewrite the projection constraint by eliminating $\lambda_i$ using the cross product $\tilde{\mathbf{p}}_i \times K(R\mathbf{P}_i+\mathbf{t}) = \mathbf{0}$. This gives 2 linear equations per point in the 12 unknowns of the projection matrix $P = K[R|\mathbf{t}]$. Stack into $A\mathbf{p}=\mathbf{0}$, solve via SVD. Then decompose $P = K[R|\mathbf{t}]$ using QR decomposition to recover $K, R, \mathbf{t}$.

4. **Limitation** — DLT does not enforce $R \in SO(3)$ — the solution may have $\det(R) \neq 1$. Fix by projecting to the nearest rotation via SVD: $R = UV^\top$ from $SVD(R_{raw}) = U\Sigma V^\top$.

### Part 3: P3P

5. **P3P geometry** — With 3 correspondences $(\mathbf{P}_1,\tilde{\mathbf{p}}_1), (\mathbf{P}_2,\tilde{\mathbf{p}}_2), (\mathbf{P}_3,\tilde{\mathbf{p}}_3)$, each back-projected ray $\mathbf{r}_i = K^{-1}\tilde{\mathbf{p}}_i$ defines a line from the camera center. The 3D point lies at $\lambda_i\mathbf{r}_i$ for some unknown depth $\lambda_i > 0$.

6. **Cosine law system** — The known 3D distances $d_{ij} = \|\mathbf{P}_i - \mathbf{P}_j\|$ must equal the distances in camera frame $\|\lambda_i\mathbf{r}_i - \lambda_j\mathbf{r}_j\|$. Expand:
$$d_{ij}^2 = \lambda_i^2 + \lambda_j^2 - 2\lambda_i\lambda_j\cos\theta_{ij}$$
where $\cos\theta_{ij} = \mathbf{r}_i^\top\mathbf{r}_j$. This gives 3 equations in 3 unknowns $(\lambda_1,\lambda_2,\lambda_3)$ — a system of quadratics with up to 4 real solutions. Describe how to solve (substitution → quartic polynomial → Sturm sequences or Gröbner basis).

7. **Disambiguation** — Use the 4th point (or reprojection error) to select the correct solution among up to 4.

### Part 4: EPnP (Efficient PnP, n ≥ 4)

8. **Control points** — EPnP expresses all $n$ 3D points as weighted sums of 4 control points $\{\mathbf{c}_j\}$ (a barycentric basis in 3D):
$$\mathbf{P}_i = \sum_{j=1}^4 w_{ij}\mathbf{c}_j, \quad \sum_j w_{ij}=1$$
The control points in camera frame $\{\mathbf{c}_j^c\}$ are the unknowns (12 values). Show the projection constraint becomes linear in $\{\mathbf{c}_j^c\}$.

9. **SVD solution** — Stack all $n$ projection constraints → $2n\times12$ matrix $M$, solve $M\mathbf{x}=\mathbf{0}$ via SVD. The solution lives in the null space of $M$ (dimension 1–4 depending on scene). Recover $\{\mathbf{c}_j^c\}$, then $\{(R,\mathbf{t})\}$ via Procrustes alignment between $\mathbf{c}_j^c$ and $\mathbf{c}_j$.

10. **Complexity** — EPnP is $O(n)$ (linear in number of points after the $O(n)$ SVD truncated to 12 columns). Compare: DLT is $O(n)$ but less accurate; iterative methods (LM refinement) are $O(nk)$ for $k$ iterations.

### Part 5: Comparison

11. **Formal table:**

| Method | Min pts | Solutions | Complexity | Use case |
|--------|---------|-----------|------------|----------|
| DLT | 6 | 1 (least sq) | $O(n)$ | Batch, rough init |
| P3P | 3 | Up to 4 | $O(1)$ | RANSAC inner loop |
| EPnP | 4 | 1 | $O(n)$ | Accurate, efficient |
| LM refinement | any | 1 | $O(nk)$ | Polish after init |

12. **RANSAC + P3P — the standard pipeline** — In ORB-SLAM and most VIO systems, localization uses:
   - Sample 3 correspondences → P3P → up to 4 poses
   - Score each pose by reprojection error on all $n$ points
   - Keep inliers → refine with EPnP or LM
   Derive why P3P is preferred inside RANSAC (minimum sample = 3 → fewest iterations needed).

---

## M16 — Reprojection Error vs Photometric Error

### Problem Statement
Two fundamentally different error metrics drive visual SLAM: reprojection error (indirect methods) and photometric error (direct methods). Derive both, their Jacobians, and formalize when each is appropriate.

### Part 1: Reprojection Error

1. **Definition** — Given a 3D map point $\mathbf{P}_i$ and camera pose $T = (R,\mathbf{t})$, the reprojection error is the distance between the observed pixel $\tilde{\mathbf{p}}_i$ and the projected pixel $\pi(T, \mathbf{P}_i)$:
$$\mathbf{e}_i = \tilde{\mathbf{p}}_i - \pi(T, \mathbf{P}_i), \quad \pi(T,\mathbf{P}) = K\frac{(R\mathbf{P}+\mathbf{t})_{xy}}{(R\mathbf{P}+\mathbf{t})_z}$$
Units: pixels. Cost: $\|\mathbf{e}_i\|^2_{\Sigma_i}$ where $\Sigma_i$ is the pixel measurement covariance.

2. **Jacobian w.r.t. pose** — Derive $\frac{\partial \mathbf{e}_i}{\partial \xi}$ where $\xi \in \mathfrak{se}(3)$ is the Lie algebra perturbation of $T$. Using the chain rule:
$$\frac{\partial \mathbf{e}_i}{\partial \xi} = -\frac{\partial \pi}{\partial \mathbf{P}_c} \cdot \frac{\partial \mathbf{P}_c}{\partial \xi}$$
Derive each factor explicitly. $\frac{\partial \pi}{\partial \mathbf{P}_c}$ is the $2\times3$ projection Jacobian (from M4). $\frac{\partial \mathbf{P}_c}{\partial \xi}$ is the $3\times6$ pose Jacobian from the adjoint.

3. **Jacobian w.r.t. map point** — Derive $\frac{\partial \mathbf{e}_i}{\partial \mathbf{P}_i}$, the $2\times3$ Jacobian w.r.t. the 3D point position. Used in bundle adjustment to simultaneously optimize poses and points.

4. **Robust kernels** — Raw least-squares is sensitive to outliers (wrong matches). Replace $\|\mathbf{e}\|^2$ with a robust cost $\rho(\|\mathbf{e}\|)$:
   - Huber: $\rho(e) = e^2/2$ if $|e|\leq k$, else $k(|e|-k/2)$
   - Cauchy: $\rho(e) = \frac{c^2}{2}\log(1 + (e/c)^2)$
   Show how this modifies the normal equations: the effective weight per residual becomes $w_i = \rho'(\|\mathbf{e}_i\|)/\|\mathbf{e}_i\|$ → iteratively reweighted least squares (IRLS).

### Part 2: Photometric Error

5. **Definition** — Direct methods minimize the difference in raw pixel intensities between a reference frame $I_r$ and current frame $I_c$ for a patch $\mathcal{P}$ around a point $\mathbf{p}$:
$$\mathbf{e}_{photo} = \sum_{\mathbf{u}\in\mathcal{P}} \left[I_r(\mathbf{u}) - I_c(\pi(T, \pi_r^{-1}(\mathbf{u}, d)))\right]^2$$
where $\pi_r^{-1}(\mathbf{u}, d)$ backprojects pixel $\mathbf{u}$ at depth $d$ to 3D, then $\pi$ re-projects into the current frame.

6. **Jacobian** — Derive the photometric Jacobian via chain rule:
$$\frac{\partial \mathbf{e}_{photo}}{\partial \xi} = -\nabla I_c \cdot \frac{\partial \pi}{\partial \mathbf{P}_c} \cdot \frac{\partial \mathbf{P}_c}{\partial \xi}$$
where $\nabla I_c \in \mathbb{R}^{1\times2}$ is the image gradient at the warped location. This is the **image Jacobian** or **interaction matrix** from visual servoing.

7. **Affine illumination model** — DSO adds per-frame affine brightness parameters $(a, b)$: $I_c \to e^{a_c} I_c + b_c$. The error becomes:
$$\mathbf{e} = I_r(\mathbf{u}) - e^{a_c - a_r}I_c(\pi(\cdot)) - (b_c - b_r)$$
Explain why this is needed (exposure changes, auto-gain cameras) and derive the extra Jacobian terms w.r.t. $(a,b)$.

### Part 3: Formal Comparison

8. **Derive and fill in:**

| Property | Reprojection Error | Photometric Error |
|----------|-------------------|------------------|
| Input | Matched keypoint pairs | Raw pixel intensities |
| Units | Pixels | Intensity values |
| Requires feature matching | Yes | No |
| Sensitive to texture | No (works on any matched point) | Yes (needs texture gradient) |
| Sensitive to illumination | No (only uses pixel location) | Yes (brightness constancy) |
| Sub-pixel accuracy | Yes (iterative) | Yes (image gradient) |
| Outlier handling | Ratio test + RANSAC + robust kernel | Hard (no explicit outlier model) |
| Used in | ORB-SLAM, VINS-Mono, MSCKF | DSO, LSD-SLAM, SVO (hybrid) |
| Computational cost | Low (sparse, few keypoints) | High (dense/semi-dense patches) |

---

## M17 — Covariance Propagation in Bundle Adjustment

### Problem Statement
After solving bundle adjustment (BA), the solution covariance quantifies the uncertainty of estimated poses and map points. Derive how covariance is computed from the BA Hessian and how it propagates.

### Part 1: BA as a Least-Squares Problem

1. **Cost function** — BA minimizes:
$$F(\mathbf{x}) = \frac{1}{2}\sum_{i,j} \mathbf{e}_{ij}^\top \Sigma_{ij}^{-1} \mathbf{e}_{ij}$$
where $\mathbf{x} = [\xi_1,\ldots,\xi_C, \mathbf{P}_1,\ldots,\mathbf{P}_N]^\top$ stacks all $C$ camera poses and $N$ map points, and $\mathbf{e}_{ij}$ is the reprojection error of point $j$ in camera $i$.

2. **Sparse Hessian** — The Hessian $H = J^\top \Sigma^{-1} J$ has block structure due to the factor graph: each residual $\mathbf{e}_{ij}$ involves only pose $i$ and point $j$, so $H$ is block-sparse. Partition:
$$H = \begin{bmatrix} H_{cc} & H_{cp} \\ H_{cp}^\top & H_{pp} \end{bmatrix}$$
where $H_{cc} \in \mathbb{R}^{6C\times6C}$ (camera-camera), $H_{pp} \in \mathbb{R}^{3N\times3N}$ (point-point, block diagonal), $H_{cp}$ (camera-point coupling).

### Part 2: Solution Covariance

3. **Covariance of the full solution** — At convergence, the solution covariance is:
$$\Sigma_{\mathbf{x}} = H^{-1} = (J^\top \Sigma^{-1} J)^{-1}$$
Inverting the full $H$ is $O((6C+3N)^3)$ — impractical for large maps. Explain why.

4. **Schur complement for marginal covariance** — To get only the camera pose covariance (marginalizing out points), use the Schur complement:
$$\Sigma_{cc} = \left(H_{cc} - H_{cp} H_{pp}^{-1} H_{cp}^\top\right)^{-1}$$
$H_{pp}$ is block diagonal → $H_{pp}^{-1}$ is $O(N)$ to compute. The reduced camera system $S = H_{cc} - H_{cp}H_{pp}^{-1}H_{cp}^\top$ is $O(6C\times6C)$. Derive this step-by-step from block matrix inversion.

5. **Uncertainty ellipsoids** — The $3\sigma$ covariance ellipsoid of pose $\xi_i$ is defined by $\{\delta\xi : \delta\xi^\top \Sigma_{ii}^{-1} \delta\xi \leq 9\}$. Its axes are the eigenvectors of $\Sigma_{ii}$ scaled by $3\sqrt{\lambda_k}$. Derive this from the eigendecomposition.

### Part 3: Propagation to Derived Quantities

6. **Propagating to a new point** — Given estimated pose $\hat{T}$ with covariance $\Sigma_T$ and a new 3D point $\hat{\mathbf{P}}$ with covariance $\Sigma_P$, propagate uncertainty to the reprojected pixel location:
$$\Sigma_{\mathbf{p}} = J_T \Sigma_T J_T^\top + J_P \Sigma_P J_P^\top$$
where $J_T = \frac{\partial \pi}{\partial \xi}$ and $J_P = \frac{\partial \pi}{\partial \mathbf{P}}$ are the pose and point Jacobians from M16. This gives the uncertainty ellipse in the image — used to gate measurements in SLAM.

7. **Information matrix form** — Explain why SLAM systems often work with the information matrix $\Lambda = \Sigma^{-1}$ rather than the covariance: marginalization is subtraction in information form (Schur), but addition of independent sources is addition in information form. Derive the equivalence.

---

## M18 — Monocular SLAM for a Drone: Variables, Factor Graph, Observability

### Problem Statement
Formalize the complete monocular SLAM problem for a drone (UAV). Define every optimization variable, the factor graph structure, initialization challenges, and the observability conditions that make the problem solvable.

### Part 1: State Definition

1. **Full state** — For a monocular camera drone, define the optimization variables:
   - **Poses**: $T_k = (R_k, \mathbf{p}_k) \in SE(3)$ for each keyframe $k = 1,\ldots,K$ — position and orientation of camera in world frame
   - **Map points**: $\mathbf{P}_j \in \mathbb{R}^3$ (or inverse depth $\rho_j \in \mathbb{R}$) for each landmark $j = 1,\ldots,N$
   - **Scale**: $s \in \mathbb{R}^+$ — monocular SLAM is scale-ambiguous; scale is a separate variable unless fixed by another sensor
   - **IMU biases** (if IMU fused): accelerometer bias $\mathbf{b}_a \in \mathbb{R}^3$, gyroscope bias $\mathbf{b}_g \in \mathbb{R}^3$
   - **Velocity** (VIO): $\mathbf{v}_k \in \mathbb{R}^3$ for each keyframe

2. **Scale ambiguity** — Prove that monocular SLAM is scale-ambiguous: if $\{T_k, \mathbf{P}_j\}$ is a solution, then $\{(R_k, s\mathbf{p}_k), s\mathbf{P}_j\}$ gives the same reprojection errors for any $s > 0$. Explain how scale is fixed in practice: known object size, IMU integration, stereo baseline, or ground plane.

### Part 2: Factor Graph

3. **Reprojection factors** — For each observation of point $j$ in keyframe $k$:
$$\mathbf{e}_{kj} = \tilde{\mathbf{p}}_{kj} - \pi(T_k, \mathbf{P}_j) \sim \mathcal{N}(0, \Sigma_{kj})$$
This is a binary factor connecting $T_k$ and $\mathbf{P}_j$. Typical: 15–20 observations per map point, 200–500 map points per keyframe.

4. **IMU preintegration factors** (for VIO drone) — Between keyframes $k$ and $k+1$, IMU measurements are preintegrated into a relative motion factor:
$$\mathbf{e}_{IMU} = \text{preint}(\mathbf{a}_{k:k+1}, \boldsymbol{\omega}_{k:k+1}) \ominus (T_{k+1} \ominus T_k, \mathbf{v}_{k+1} - R_k\mathbf{g}\Delta t)$$
Explain why preintegration (Forster et al.) is used: avoids re-integrating raw IMU when poses change during optimization.

5. **Loop closure factors** — When the drone revisits a location, a relative pose factor $T_k \ominus T_l \sim \mathcal{N}(\hat{T}_{kl}, \Sigma_{lc})$ is added between distant keyframes. This is a binary factor with high uncertainty (from place recognition) but crucial for global consistency.

6. **Prior factor** — To fix gauge freedom (global position and orientation unobservable in monocular SLAM), add a prior on the first pose: $T_0 = I$ with small covariance.

### Part 3: Initialization

7. **The monocular initialization problem** — The first two keyframes cannot be initialized from a single frame — depth is completely unobservable. Describe the standard pipeline:
   - Accumulate frames until enough baseline (parallax $>$ threshold, e.g. 30px median flow)
   - Estimate $F$ or $E$ matrix from 8 correspondences → recover $(R, \mathbf{t})$ up to scale
   - Triangulate initial map points
   - Run full BA to refine
   - Set scale arbitrarily (e.g. $\|\mathbf{t}\|=1$) or fix with IMU

8. **Degeneracies** — State when initialization fails:
   - Pure rotation (no translation): $F$ matrix is degenerate, triangulation fails
   - Planar scene: $F$ has spurious solutions, use homography $H$ instead
   - Slow drone hovering: insufficient parallax → depth uncertainty huge → don't initialize

### Part 4: Observability

9. **What is observable in monocular SLAM** — Formally, a quantity is observable if it can be uniquely recovered from the measurements. Derive:
   - **Observable**: relative poses between keyframes, relative 3D structure (shape), relative scale between different motion segments
   - **Unobservable (gauge freedom)**: global position (4 DOF — $x,y,z$ translation + yaw), absolute scale
   - **With IMU**: absolute roll and pitch become observable (gravity vector), scale becomes observable (accelerometer gives absolute force), but global $x,y,z$ and yaw remain unobservable

10. **FEJ (First Estimates Jacobian)** — In EKF-based monocular SLAM, linearizing at the current estimate creates spurious information (inconsistency). FEJ fixes this by always evaluating Jacobians at the first estimate. Explain why this preserves the correct null space of the information matrix (the unobservable directions).

11. **Optimization variable count (example)** — For a drone with $K=50$ keyframes, $N=1000$ map points, with IMU:
    - Poses: $50 \times 6 = 300$
    - Velocities: $50 \times 3 = 150$
    - IMU biases: $50 \times 6 = 300$
    - Map points: $1000 \times 3 = 3000$
    - **Total: 3750 variables**
    Estimate the Hessian size and explain why Schur complement (M17) is essential.

---

## M19 — Stereo Depth Estimation and Covariance Propagation

### Problem Statement
A stereo camera recovers metric depth by triangulating matched points across two calibrated cameras with known baseline. Derive the depth formula, its uncertainty, and how errors propagate.

### Tasks

1. **Stereo geometry** — Two cameras separated by baseline $b$ (horizontal). Both have focal length $f$, principal point $c_x$. A 3D point $\mathbf{P} = [X,Y,Z]^\top$ projects to pixel $u_L = f X/Z + c_x$ in the left camera and $u_R = f(X-b)/Z + c_x$ in the right. Define disparity $d = u_L - u_R$ and derive:
$$Z = \frac{fb}{d}, \quad X = \frac{Z(u_L - c_x)}{f}, \quad Y = \frac{Z(v - c_y)}{f}$$

2. **Disparity uncertainty** — Disparity is estimated by block matching or learned stereo. Model disparity error $\sigma_d$ (typically 0.5–1 pixel RMS). Propagate to depth uncertainty via the delta method:
$$\sigma_Z = \left|\frac{\partial Z}{\partial d}\right|\sigma_d = \frac{fb}{d^2}\sigma_d = \frac{Z^2}{fb}\sigma_d$$
Key result: **depth uncertainty grows as $Z^2$** — close objects are accurate, far objects are noisy.

3. **Full 3D covariance** — Propagate $\sigma_{u_L}, \sigma_{u_R}, \sigma_v$ (pixel noise, all $\sim\sigma_{px}$) to $[X,Y,Z]$ via Jacobian $J = \frac{\partial [X,Y,Z]^\top}{\partial [u_L,u_R,v]^\top}$. Derive $J$ and compute $\Sigma_{3D} = J\Sigma_{pixel}J^\top$. Show the depth-direction uncertainty dominates at range.

4. **Maximum reliable depth** — Define the maximum depth where $\sigma_Z/Z < \epsilon$ (e.g. 10% relative error). Derive:
$$Z_{max} = \frac{fb\,\epsilon}{\sigma_d}$$
For $f=500\text{px}$, $b=0.1\text{m}$, $\sigma_d=1\text{px}$, $\epsilon=0.1$: $Z_{max} = 5\text{m}$. Explain how to extend range: increase baseline, increase focal length.

5. **Stereo vs monocular** — Stereo gives metric depth in a single frame (no motion needed). Monocular requires motion + triangulation. Derive the effective baseline in monocular SLAM after moving distance $t$: equivalent to stereo with $b = t$, but depth is only available after motion.

---

## M20 — FEJ: First Estimates Jacobian

### Problem Statement
In EKF-based SLAM, linearization at the current (updated) estimate introduces spurious information along unobservable directions, causing filter inconsistency. FEJ fixes this. Derive with a concrete simple example.

### Tasks

1. **The inconsistency problem** — Consider a 1D robot at position $x$ observing a landmark at position $\ell$. The measurement is $z = \ell - x + n$, $n\sim\mathcal{N}(0,\sigma^2)$. In EKF, the Jacobian w.r.t. state $[x, \ell]^\top$ is $H = [-1, +1]$. Show that the null space of $H$ is $[1,1]^\top$ — meaning uniform translation of robot and landmark is unobservable (correct). The EKF information added per update is $H^\top R^{-1} H$ — show its null space is also $[1,1]^\top$. So far consistent.

2. **Where inconsistency enters** — After an EKF update, the state estimate changes from $[\hat{x}_1, \hat{\ell}_1]$ to $[\hat{x}_2, \hat{\ell}_2]$. The next Jacobian is evaluated at the new estimate. For a nonlinear measurement $z = h(x,\ell)$, the new Jacobian $H_2 \neq H_1$ has a **different null space**. The summed information $H_1^\top R^{-1}H_1 + H_2^\top R^{-1}H_2$ has a null space that is smaller than either — the filter gains information in the unobservable direction spuriously. Derive this for a simple nonlinear case.

3. **FEJ fix** — Always evaluate the Jacobian at the **first estimate** $[\hat{x}_0, \hat{\ell}_0]$ rather than the current estimate. This ensures all Jacobians have the same null space → the summed information matrix preserves the true unobservable directions. Formalize: $H_k = \left.\frac{\partial h}{\partial [x,\ell]}\right|_{[\hat{x}_0, \hat{\ell}_0]}$ for all $k$.

4. **Concrete 2D example** — Robot at $[0,0]$, landmark at $[5,0]$. Observation: bearing angle $\theta = \text{atan2}(\ell_y - x_y, \ell_x - x_x)$. Compute $H$ at the true position, then at a perturbed position after an update. Show numerically that the null space shifts (inconsistency) and that FEJ (using first estimate) prevents this.

5. **Trade-off** — FEJ uses a fixed linearization point → linearization error accumulates over time but the filter remains consistent. Standard EKF re-linearizes → smaller linearization error per step but inconsistent. Explain why consistency (correct covariance) is more important for long-term SLAM than small per-step linearization error.

---

## M21 — Sliding Window BA and Marginalization

### Problem Statement
Full BA over all keyframes is computationally intractable for real-time systems. Sliding window optimization keeps a fixed window of recent keyframes and marginalizes old ones into a prior. Derive the full procedure.

### Tasks

1. **Sliding window setup** — Keep $W$ most recent keyframes. At each step: add a new keyframe → window size $W+1$ → marginalize the oldest keyframe → window size back to $W$.

2. **Variables to marginalize** — Partition the state $\mathbf{x} = [\mathbf{x}_m, \mathbf{x}_r]$ where $\mathbf{x}_m$ = variables to marginalize (old pose + its map points seen only from old frames) and $\mathbf{x}_r$ = remaining variables.

3. **Marginalization via Schur complement** — The joint information matrix:
$$\Lambda = \begin{bmatrix} \Lambda_{mm} & \Lambda_{mr} \\ \Lambda_{mr}^\top & \Lambda_{rr} \end{bmatrix}$$
Marginalizing $\mathbf{x}_m$ gives the **Schur complement prior** on $\mathbf{x}_r$:
$$\Lambda_{prior} = \Lambda_{rr} - \Lambda_{mr}^\top \Lambda_{mm}^{-1} \Lambda_{mr}$$
$$\mathbf{b}_{prior} = \mathbf{b}_r - \Lambda_{mr}^\top \Lambda_{mm}^{-1} \mathbf{b}_m$$
Derive this from Gaussian conditioning. Show it is equivalent to integrating out $\mathbf{x}_m$ from the joint Gaussian.

4. **Linearization point problem** — The prior $\Lambda_{prior}$ is computed at a specific linearization point $\bar{\mathbf{x}}_r$. If $\mathbf{x}_r$ changes significantly during subsequent optimization, the prior becomes inconsistent. Solutions: FEJ (fix linearization point), or limit the window size so drift is small.

5. **What gets marginalized** — Explain the decision: marginalize the oldest keyframe's pose and the map points that are **only** observed from that frame (not from any remaining frames). Points observed from multiple remaining frames are kept in the window as active landmarks.

6. **Computational savings** — Without marginalization: $O(W^3)$ per solve grows with history. With sliding window: $O(W^3)$ bounded, plus $O(1)$ marginalization step. Derive that the marginalization step is $O(n_m^3 + n_m^2 n_r)$ where $n_m$ = marginalized variables, $n_r$ = remaining variables.

7. **VINS-Mono implementation** — Describe how VINS-Mono uses this: $W=10$ keyframes, marginalize either the oldest frame (when moving) or the second-newest frame (when nearly stationary, to keep a reference frame). Explain the two-policy design from the perspective of information content.

---

## M22 — Depth Estimation in Monocular SLAM

### Problem Statement
Monocular SLAM cannot observe depth from a single frame. Derive all methods used to recover depth from motion, understand their uncertainty models, and formalize the depth filter used in SVO and DSO.

### Tasks

1. **Why monocular depth is ambiguous** — Prove: for any 3D point $\mathbf{P}$ and scale $s$, the point $s\mathbf{P}$ produces the same 2D projection if all camera translations are also scaled by $s$. Depth is only observable relative to the camera baseline.

2. **Triangulation from two views** — Given two camera poses $T_1, T_2$ and a matched pixel pair, set up the triangulation system (from M13/M14). Derive the depth uncertainty as a function of:
   - Baseline $\|t_{12}\|$: larger baseline → smaller depth uncertainty
   - Pixel noise $\sigma_{px}$: larger noise → larger uncertainty
   - Viewing angle $\alpha$ (angle between the two rays): derive $\sigma_Z \propto \frac{Z^2 \sigma_{px}}{f \|t_{12}\| \sin\alpha}$

3. **Multi-view depth filter (SVO/DSO approach)** — As the camera moves, each new frame provides a new depth measurement for tracked points. Model the depth along a ray as a scalar $d$ with a Gaussian belief $\mathcal{N}(\mu_d, \sigma_d^2)$. Each new observation gives a noisy measurement $\hat{d}$ with uncertainty $\sigma_{obs}$. Bayesian update:
$$\mu_d \leftarrow \frac{\sigma_{obs}^2 \mu_d + \sigma_d^2 \hat{d}}{\sigma_d^2 + \sigma_{obs}^2}, \quad \frac{1}{\sigma_d^2} \leftarrow \frac{1}{\sigma_d^2} + \frac{1}{\sigma_{obs}^2}$$
Show this is the 1D KF update. Depth variance decreases monotonically with each new observation.

4. **Convergence criterion** — Declare a depth estimate converged when $\sigma_d < \tau$ (e.g. 1% of $\mu_d$). Derive: the number of observations needed to converge from initial $\sigma_0$ is $N \approx (\sigma_0/\tau)^2$ if each observation has equal uncertainty.

5. **Outlier-robust depth filter (Vogiatzis)** — The Gaussian filter is sensitive to outliers (wrong matches). SVO uses a mixture model: depth is either an inlier (Gaussian) or uniform outlier:
$$p(d) = \pi \mathcal{N}(d;\mu,\sigma^2) + (1-\pi)\mathcal{U}(d; d_{min}, d_{max})$$
Update both $(\mu, \sigma^2)$ and the inlier probability $\pi$ using Bayes' rule. Derive the E-step (soft assignment) and M-step (parameter update) — this is a 1D EM algorithm.

6. **Depth from IMU preintegration** — When IMU is available, the camera translation between frames is known in metric scale from IMU integration. This provides an absolute baseline → metric depth directly from triangulation, without the scale ambiguity of pure monocular.

7. **Comparison of depth sources in monocular SLAM:**

| Method | Metric scale | Requires motion | Accuracy | Used in |
|--------|-------------|-----------------|----------|---------|
| Triangulation (2-view) | No | Yes (parallax) | Medium | ORB-SLAM init |
| Depth filter (multi-view) | No | Yes (accumulate) | High (converged) | SVO, DSO |
| IMU-aided triangulation | Yes | Yes | High | VINS-Mono, OKVIS |
| Known object size | Yes | No | Depends | AR, special cases |
| Ground plane assumption | Partial | No | Low | Autonomous driving |

---

## M23 — Covisibility Graph

### Problem Statement
The covisibility graph is the key data structure in ORB-SLAM that enables scalable local BA, keyframe culling, and loop closure. Formalize it and derive how it's used.

### Tasks

1. **Definition** — A covisibility graph $G = (V, E)$ where:
   - **Nodes** $V$: each keyframe $K_i$ is a node
   - **Edges** $E$: an edge $(K_i, K_j)$ exists if keyframes $i$ and $j$ share at least $\theta$ co-observed map points (typically $\theta = 15$). The edge weight $w_{ij}$ = number of shared map points.
   Formalize: $w_{ij} = |\mathcal{P}_i \cap \mathcal{P}_j|$ where $\mathcal{P}_i$ is the set of map points observed by keyframe $i$.

2. **Construction** — When a new keyframe $K_{new}$ is inserted, update the graph:
   - For each map point $\mathbf{P}_j$ observed by $K_{new}$, find all other keyframes $K_i$ that also observe $\mathbf{P}_j$
   - Increment $w_{new,i}$ by 1
   - If $w_{new,i} \geq \theta$: add/update edge $(K_{new}, K_i)$
   Complexity: $O(N_{pts} \cdot \bar{k})$ where $N_{pts}$ = points in new keyframe, $\bar{k}$ = mean number of keyframes per point.

3. **Local BA with covisibility** — ORB-SLAM's local BA optimizes the current keyframe, its covisibility neighbors (top-$k$ by weight), and all map points seen by them. Fixed (not optimized): keyframes connected to the local window but outside it (they provide constraints without drifting). Derive why this bound is necessary (prevents unbounded optimization).

4. **Spanning tree (essential graph)** — The minimum spanning tree of the covisibility graph (keeping only the highest-weight edge per node) is the **essential graph** — used for pose graph optimization after loop closure. Fewer edges → faster optimization; high-weight edges → most informative connections kept.

5. **Keyframe culling** — A keyframe $K_i$ is redundant if 90% of its map points are seen by at least 3 other keyframes. The covisibility graph identifies which keyframes share the most observations, making culling decisions efficient. Derive why removing redundant keyframes improves speed without losing accuracy.

6. **Loop closure via covisibility** — When a loop candidate keyframe $K_l$ is detected (by DBoW2), its covisibility neighbors are used to verify the loop (geometric consistency check across multiple keyframes) and to fuse duplicate map points after the loop is closed.

---

## M24 — Pose Graph vs Factor Graph

### Problem Statement
Both represent the SLAM problem as a graph but at different levels of abstraction. Derive both from the MAP estimation problem, formalize nodes/edges, and understand the trade-offs.

### Tasks

### Part 1: Factor Graph

1. **Definition** — A factor graph is a bipartite graph $G = (V \cup F, E)$:
   - **Variable nodes** $V$: all optimization variables (poses $T_k$, map points $\mathbf{P}_j$, biases, velocities)
   - **Factor nodes** $F$: each factor $f_i$ encodes a probabilistic constraint (measurement, prior, IMU)
   - **Edges**: factor $f_i$ connects to exactly the variables it involves

2. **Joint probability** — The MAP problem factorizes as:
$$p(\mathbf{x} | \mathbf{z}) \propto \prod_i f_i(\mathbf{x}_i)$$
where each factor $f_i(\mathbf{x}_i) = \exp(-\frac{1}{2}\|\mathbf{e}_i(\mathbf{x}_i)\|^2_{\Sigma_i})$ for a Gaussian factor. Minimizing the negative log gives the least-squares problem. Draw the factor graph for a simple 3-pose, 2-landmark SLAM problem.

3. **Types of factors** — Enumerate and formalize:
   - **Reprojection factor**: binary, connects $T_k$ and $\mathbf{P}_j$, residual = pixel error $\in\mathbb{R}^2$
   - **IMU preintegration factor**: binary, connects $T_k$ and $T_{k+1}$ (+ velocities + biases), residual $\in\mathbb{R}^{15}$
   - **Loop closure factor**: binary, connects $T_i$ and $T_j$, residual = relative pose error $\in\mathbb{R}^6$
   - **Prior factor**: unary, connects $T_0$, residual = deviation from initial guess $\in\mathbb{R}^6$
   - **Marginalization prior**: dense factor connecting all remaining variables after marginalization

4. **Sparsity** — The Hessian of the factor graph is sparse because each factor involves only a few variables. Derive that the number of non-zeros in $H$ is $O(C + N)$ (not $O((C+N)^2)$) for typical SLAM. This is why sparse solvers (CHOLMOD, g2o, Ceres) are essential.

### Part 2: Pose Graph

5. **Definition** — A pose graph is a special case of a factor graph where **map points are marginalized out**, leaving only:
   - **Nodes**: camera poses $\{T_k\}$
   - **Edges**: relative pose constraints between connected poses, with associated covariance $\Sigma_{ij}$

6. **Edge construction** — A pose graph edge $(i,j)$ encodes the relative transformation $\hat{T}_{ij} = T_i^{-1} T_j$ estimated from either:
   - Sequential keyframes (odometry edge): from VO/VIO
   - Loop closure (loop edge): from place recognition + geometric verification
   Residual: $\mathbf{e}_{ij} = \log(T_i^{-1} T_j \hat{T}_{ij}^{-1}) \in \mathfrak{se}(3)$ (Lie algebra error).

7. **Pose graph optimization** — Minimize:
$$F = \sum_{(i,j)\in E} \mathbf{e}_{ij}^\top \Sigma_{ij}^{-1} \mathbf{e}_{ij}$$
Solve with LM or Dogleg (g2o, GTSAM). This is faster than full BA because map points are absent — the Hessian is $6C\times6C$ (only poses), not $(6C+3N)\times(6C+3N)$.

8. **When pose graph is used vs full BA:**

| | Full BA (factor graph with points) | Pose graph |
|--|-----------------------------------|----|
| Variables | Poses + map points | Poses only |
| Accuracy | Higher (points refine geometry) | Lower (points marginalized = information loss) |
| Speed | Slower ($O((C+N)^3)$) | Faster ($O(C^3)$) |
| Use case | Local refinement (sliding window) | Global loop closure correction |
| In ORB-SLAM3 | Local BA, map merging | Loop closing (pose graph opt first, then full BA) |

9. **The standard pipeline** — Derive why ORB-SLAM does pose graph optimization first after loop detection, then full BA: pose graph quickly corrects global trajectory drift (seconds), then full BA re-refines map points (minutes in background). Doing full BA immediately would be too slow for real-time.

---

## M25 — iSAM and iSAM2: Incremental Smoothing and Mapping

### Problem Statement
Full BA re-solves from scratch each time a new measurement arrives — $O((C+N)^3)$ per step. iSAM (Kaess et al., 2008) and iSAM2 (Kaess et al., 2012) update the solution **incrementally** by exploiting the Bayes tree data structure. Derive the key ideas.

### Part 1: Batch SLAM as a Linear Algebra Problem

1. **QR factorization of the Jacobian** — The BA normal equations $H\mathbf{x} = \mathbf{b}$ (where $H = J^\top\Sigma^{-1}J$) can be solved via QR factorization of the weighted Jacobian $\tilde{J} = \Sigma^{-1/2}J$:
$$\tilde{J} = QR \implies H = R^\top R \text{ (Cholesky)}$$
The upper triangular $R$ (square root information matrix) is sufficient to represent the full posterior. Derive that solving $R^\top R \mathbf{x} = \mathbf{b}$ via back-substitution is $O(n^2)$ once $R$ is known.

2. **Variable ordering matters** — The sparsity of $R$ depends on variable ordering. A good ordering (e.g. COLAMD — Column Approximate Minimum Degree) minimizes fill-in during QR. Explain why: eliminating a variable with $k$ neighbors creates a clique of size $k$ — small $k$ = less fill-in.

### Part 2: iSAM — Incremental QR Updates

3. **The iSAM insight** — When a new measurement arrives (new keyframe + observations), it adds new rows to $\tilde{J}$. Instead of recomputing QR from scratch, update $R$ incrementally using **Givens rotations** to zero out the new entries. Each Givens rotation is $O(n)$ — total update is $O(kn)$ where $k$ = number of new non-zeros. Compare to batch: $O(n^3)$ for full QR.

4. **Periodically re-order and refactor** — Fill-in accumulates over time as new variables are added. iSAM periodically performs a full batch update to reset fill-in. Derive the amortized cost: batch every $B$ steps → cost per step is $O(kn + n^3/B)$.

5. **Limitation of iSAM** — Variable reordering (to reduce fill-in) requires re-factorizing the entire $R$ — expensive. iSAM2 eliminates this by using the Bayes tree.

### Part 3: iSAM2 — Bayes Tree

6. **From factor graph to Bayes net** — A factor graph can be converted to a Bayes net (directed) by **variable elimination**: eliminate variables one at a time, each creating a conditional density. The elimination order determines the structure.

7. **Bayes tree** — The Bayes tree is a tree of **cliques** (groups of variables), where each clique stores the conditional $p(\text{frontal} | \text{separator})$. The root clique has no separator (it's the marginal of a subset of variables). Formally:
$$p(\mathbf{x}) = \prod_{\text{cliques}} p(\mathbf{F}_c | \mathbf{S}_c)$$
where $\mathbf{F}_c$ = frontal variables (unique to clique $c$), $\mathbf{S}_c$ = separator variables (shared with parent).

8. **Incremental update in the Bayes tree** — When a new factor (measurement) is added:
   - Find the **affected cliques**: only cliques containing variables involved in the new factor
   - **Remove** affected cliques from the tree → convert back to factor graph fragment
   - **Re-eliminate** only the fragment with updated ordering
   - **Reattach** the new cliques to the tree
   Key result: only a small subtree needs updating — $O(\log n)$ cliques in typical SLAM, not all $O(n)$.

9. **Fluid relinearization** — iSAM2 maintains a **non-linear** factor graph and relinearizes factors selectively: only re-linearize factors where the variable has moved by more than a threshold $\delta$ since last linearization. This is more principled than FEJ (M20) and avoids accumulation of linearization error.

10. **Back-substitution for the full solution** — After updating the Bayes tree, recover the full MAP estimate by a single top-down pass through the tree (back-substitution). Cost: $O(n)$ per variable.

### Part 4: Comparison

11. **Formal table:**

| Property | Batch BA | iSAM | iSAM2 |
|----------|----------|------|-------|
| Per-step complexity | $O(n^3)$ | $O(kn)$ amortized | $O(\log n)$ cliques |
| Relinearization | Full, every step | Periodic batch | Selective per variable |
| Variable reordering | Anytime | Costly (full refactor) | Free (Bayes tree) |
| Loop closure handling | Natural | Costly (dense fill) | Efficient (subtree update) |
| Implementation | Ceres, g2o | iSAM (GTSAM v1) | GTSAM, iSAM2 |
| Real-time capability | No (large maps) | Partial | Yes |

12. **Why iSAM2 is the gold standard** — Summarize: it provides the full MAP solution (not just recent window), handles loop closure incrementally, relinearizes selectively, and runs in $O(\log n)$ amortized time. Used in GTSAM, which underlies many modern SLAM systems (VINS-Fusion, Kimera, LIOSAM).

---

## M26 — Loop Closure: DBoW3, NetVLAD, CosPlace

### Problem Statement
Loop closure detects when the robot revisits a known place and corrects accumulated drift. Formalize the full pipeline — place recognition, geometric verification, and correction — and compare classical (DBoW3) vs learned (NetVLAD, CosPlace) approaches.

### Part 1: What is Loop Closure

1. **The drift correction problem** — SLAM accumulates error over time. A 1° heading error at 100m travel = 1.7m lateral drift. Formally: without loop closure the posterior $p(\mathbf{x}_{0:K})$ grows in uncertainty proportional to path length. Loop closure adds a constraint between the current pose $T_k$ and a past pose $T_j$, tightening the posterior.

2. **The two-stage pipeline** — Formalize:
   - **Stage 1 — Place recognition**: given current image $I_k$, find candidate past images $\{I_{j_1}, I_{j_2}, \ldots\}$ from the map database that show the same place. Output: ranked list of candidate keyframe IDs with similarity scores.
   - **Stage 2 — Geometric verification**: for each candidate, verify with PnP + RANSAC (M15) that a consistent relative pose $\hat{T}_{kj}$ exists with enough inliers. Reject false positives.
   - **Stage 3 — Graph correction**: add loop edge $(k, j)$ to the factor graph, run pose graph optimization (M24).

### Part 2: DBoW3 (Classical — Bag of Words)

3. **Visual vocabulary** — DBoW3 builds a $k$-ary tree (typically $k=10$, depth $d=6$ → $10^6$ visual words) by clustering ORB descriptors offline using k-means at each level. Each leaf node is a **visual word** $w_i$. Describe the offline training procedure.

4. **Image representation** — Each image $I$ is described by a **Bag of Words (BoW) vector** $\mathbf{v}_I \in \mathbb{R}^{|W|}$:
   - Extract ORB descriptors from $I$
   - Assign each descriptor to its nearest visual word (tree traversal: $O(d\log k)$ per descriptor)
   - $v_I(w) = \text{tf-idf}(w, I) = \frac{n_w}{N} \cdot \log\frac{|D|}{n_w^D}$
   where $n_w$ = count of word $w$ in $I$, $N$ = total descriptors, $|D|$ = database size, $n_w^D$ = images containing $w$. Derive why tf-idf downweights common words (low discriminability).

5. **Similarity score** — Compare two images via L1 distance of BoW vectors:
$$s(I_a, I_b) = 1 - \frac{1}{2}\|\mathbf{v}_a - \mathbf{v}_b\|_1$$
DBoW3 uses an inverted index: for each word $w$, store the list of images containing it — retrieval is $O(\text{query words})$ not $O(|D|)$.

6. **Temporal consistency check** — Reject candidates that are too close in time (likely just sequential frames, not a real loop). Only accept candidates with $|k - j| > \delta_t$ (typically 30 keyframes apart).

7. **DBoW3 failure modes** — BoW ignores spatial layout of features (same words, different arrangement = same score). Sensitive to viewpoint changes $>30°$. Vocabulary must be trained on similar domain data.

### Part 3: NetVLAD (Learned Global Descriptor)

8. **VLAD (Vector of Locally Aggregated Descriptors)** — Classical VLAD improves on BoW by encoding residuals, not just counts. For each descriptor $\mathbf{x}_i$ assigned to cluster $c_k$:
$$V(j,k) = \sum_{i: \text{nn}(\mathbf{x}_i)=k} (\mathbf{x}_i(j) - \mu_k(j))$$
Concatenate residuals for all clusters → $K \times D$ matrix, flatten + L2-normalize → global descriptor $\in\mathbb{R}^{KD}$.

9. **NetVLAD — differentiable VLAD** — Replace hard assignment $\text{nn}(\mathbf{x}_i) = k$ with soft assignment via softmax:
$$\bar{a}_k(\mathbf{x}_i) = \frac{e^{\mathbf{w}_k^\top\mathbf{x}_i + b_k}}{\sum_{k'} e^{\mathbf{w}_{k'}^\top\mathbf{x}_i + b_{k'}}}$$
The VLAD aggregation becomes differentiable → train end-to-end. The CNN backbone (VGG-16 truncated at `conv5`) extracts local descriptors, NetVLAD pools them into a global 4096-dim vector.

10. **Training — triplet loss** — NetVLAD is trained on Pittsburgh250k (Google Street View) with GPS labels. For each query $q$, positives $\mathcal{P}$ (within 25m GPS) and negatives $\mathcal{N}$ (beyond 25m). Triplet loss:
$$\mathcal{L} = \sum_{q} \sum_{p\in\mathcal{P}} \sum_{n\in\mathcal{N}} \max(0,\ d(q,p)^2 - d(q,n)^2 + m)$$
where $d(\cdot,\cdot)$ is L2 distance between global descriptors and $m$ is a margin. Hard negative mining: only sample negatives that the current model confuses (close in descriptor space but far in GPS).

### Part 4: CosPlace

11. **The domain gap problem** — NetVLAD trained on Street View fails on indoor/aerial/night imagery. CosPlace (Berton et al., 2022) reformulates place recognition as **classification** during training (not metric learning), which generalizes better across domains.

12. **Training formulation** — Partition the training map into geographical cells (e.g. $\Delta_{lat/lon}$ grid). Each cell is a class. Train a CNN to classify which cell an image was taken in using CosFace loss (cosine margin softmax):
$$\mathcal{L} = -\log\frac{e^{s(\cos\theta_y - m)}}{e^{s(\cos\theta_y - m)} + \sum_{j\neq y} e^{s\cos\theta_j}}$$
where $\theta_y$ is the angle between the image embedding and the class prototype, $m$ is a margin, $s$ is a scale. No pairs/triplets needed — simpler to train, better convergence.

13. **Why it generalizes better** — Classification loss explicitly pushes embeddings of different cells apart globally (not just per-triplet). Cell overlap is handled by training on multiple scales of the grid. The resulting descriptor generalizes to unseen domains (night, indoor, aerial) without fine-tuning.

14. **Output** — A single global descriptor (ResNet backbone, 512-dim default). Retrieval via approximate nearest neighbor search (FAISS) in the descriptor database.

### Part 5: Comparison

15. **Formal table:**

| Property | DBoW3 | NetVLAD | CosPlace |
|----------|-------|---------|----------|
| Descriptor type | BoW over ORB (sparse) | Dense CNN → VLAD pool (4096-dim) | Dense CNN (512-dim) |
| Training data needed | Visual vocabulary (offline k-means) | GPS-labeled Street View | GPS-labeled map images |
| Training objective | None (unsupervised clustering) | Triplet loss | CosFace classification |
| Viewpoint robustness | Low ($<30°$) | Medium | Medium-High |
| Domain generalization | Low (vocabulary domain-specific) | Low-Medium | High |
| Query time | $O(\text{words})$ + inverted index | $O(d_{desc})$ ANN search | $O(d_{desc})$ ANN search |
| Used in | ORB-SLAM2/3 (default) | Research VPR benchmarks | State-of-the-art VPR |
| GPU required | No | Yes (inference) | Yes (inference) |

16. **Geometric verification after retrieval** — Regardless of place recognition method, geometric verification is mandatory:
    - Take top-5 candidates from retrieval
    - For each: match local features (ORB or SuperPoint) between query and candidate
    - Run P3P + RANSAC to find pose $(R,\mathbf{t})$ with $>30$ inliers
    - Accept loop only if inlier ratio $> 0.5$ and translation reasonable
    Explain why this step filters $>90\%$ of false positives from the retrieval stage.

---

## M27 — IMU Preintegration: Derivation and Covariance Propagation

### Problem Statement
An IMU measures accelerations and angular velocities at high rate (~200Hz). Preintegration (Forster et al., 2015) compactly summarizes raw IMU measurements between two keyframes into a single relative motion factor, enabling efficient re-use during optimization. Derive the full model.

### Tasks

1. **IMU measurement model** — The raw measurements are corrupted by noise and slowly varying biases:
$$\tilde{\boldsymbol{\omega}} = \boldsymbol{\omega} + \mathbf{b}_g + \mathbf{n}_g, \quad \tilde{\mathbf{a}} = R^\top(\mathbf{a} - \mathbf{g}) + \mathbf{b}_a + \mathbf{n}_a$$
where $\boldsymbol{\omega}$ = true angular velocity, $\mathbf{a}$ = true acceleration in world frame, $\mathbf{g}$ = gravity vector, $\mathbf{b}_g, \mathbf{b}_a$ = biases, $\mathbf{n}_g, \mathbf{n}_a \sim \mathcal{N}(0, \sigma^2 I)$ = white noise. State the continuous-time stochastic differential equations for bias evolution: $\dot{\mathbf{b}}_g = \mathbf{n}_{bg}$, $\dot{\mathbf{b}}_a = \mathbf{n}_{ba}$ (random walk).

2. **Naive integration** — Integrate measurements to get relative pose between keyframes $i$ and $j$. Show that naively integrating in world frame couples the estimate to $R_i$ (the initial rotation): if $R_i$ changes during optimization, you must re-integrate all IMU measurements → $O(N_{IMU})$ per optimization step. This is the problem preintegration solves.

3. **Preintegration in body frame** — Integrate in the IMU body frame to decouple from the initial pose. Define preintegrated measurements:
$$\Delta R_{ij} = \prod_{k=i}^{j-1} \text{Exp}((\tilde{\boldsymbol{\omega}}_k - \mathbf{b}_g)\Delta t)$$
$$\Delta \mathbf{v}_{ij} = \sum_{k=i}^{j-1} \Delta R_{ik}(\tilde{\mathbf{a}}_k - \mathbf{b}_a)\Delta t$$
$$\Delta \mathbf{p}_{ij} = \sum_{k=i}^{j-1} \left[\Delta \mathbf{v}_{ik}\Delta t + \frac{1}{2}\Delta R_{ik}(\tilde{\mathbf{a}}_k - \mathbf{b}_a)\Delta t^2\right]$$
Show these depend only on raw measurements and biases, not on $R_i, \mathbf{v}_i, \mathbf{p}_i$.

4. **Residual factor** — The IMU factor residual connecting states $(R_i,\mathbf{v}_i,\mathbf{p}_i,\mathbf{b}_i)$ and $(R_j,\mathbf{v}_j,\mathbf{p}_j,\mathbf{b}_j)$:
$$\mathbf{r}_R = \text{Log}(\Delta R_{ij}^\top R_i^\top R_j)$$
$$\mathbf{r}_v = R_i^\top(\mathbf{v}_j - \mathbf{v}_i - \mathbf{g}\Delta t_{ij}) - \Delta\mathbf{v}_{ij}$$
$$\mathbf{r}_p = R_i^\top(\mathbf{p}_j - \mathbf{p}_i - \mathbf{v}_i\Delta t_{ij} - \frac{1}{2}\mathbf{g}\Delta t_{ij}^2) - \Delta\mathbf{p}_{ij}$$
Total residual $\mathbf{r} \in \mathbb{R}^{15}$ (3+3+3 + 3+3 bias). Derive the $15\times15$ Jacobians w.r.t. the two states.

5. **Covariance propagation** — The preintegrated covariance $\Sigma_{ij} \in \mathbb{R}^{9\times9}$ (for $\Delta R, \Delta v, \Delta p$) propagates via:
$$\Sigma_{k+1} = A_k \Sigma_k A_k^\top + B_k \Sigma_{\eta} B_k^\top$$
where $A_k$ is the state transition Jacobian, $B_k$ maps noise to state, $\Sigma_\eta = \text{diag}(\sigma_{ng}^2 I, \sigma_{na}^2 I)/\Delta t$. Derive $A_k$ and $B_k$ explicitly by differentiating the integration equations.

6. **Bias correction without re-integration** — When biases $\mathbf{b}$ change slightly during optimization (e.g. by $\delta\mathbf{b}$), recompute $\Delta R_{ij}, \Delta\mathbf{v}_{ij}, \Delta\mathbf{p}_{ij}$ using a first-order correction:
$$\Delta R_{ij}(\mathbf{b}+\delta\mathbf{b}) \approx \Delta R_{ij}(\mathbf{b}) \cdot \text{Exp}\left(\frac{\partial \Delta R_{ij}}{\partial \mathbf{b}_g}\delta\mathbf{b}_g\right)$$
Derive the Jacobians $\frac{\partial \Delta R}{\partial \mathbf{b}_g}$, $\frac{\partial \Delta\mathbf{v}}{\partial \mathbf{b}}$, $\frac{\partial \Delta\mathbf{p}}{\partial \mathbf{b}}$ and show they accumulate during integration — precompute once, reuse at every optimization iteration.

---

## M28 — GRU/RNN as IMU Integrator vs Classical RK4

### Problem Statement
TLIO (Yan et al., 2020) and similar works replace classical IMU integration (Runge-Kutta) with a trained GRU network that directly predicts displacement and uncertainty from raw IMU sequences. Derive why learned integration can outperform classical methods.

### Tasks

1. **Classical integration failure modes** — Classical preintegration (M27) fails when:
   - Gyroscope bias $\mathbf{b}_g$ is poorly calibrated → rotation error → wrong gravity direction → accelerometer projects gravity into horizontal → spurious horizontal acceleration → position drift
   - Pedestrian/wheeled robot: IMU on foot/wheel sees nonlinear vibrations not modeled by Gaussian noise
   - High vibration (drone propellers): non-Gaussian noise corrupts the Gaussian preintegration covariance

2. **GRU architecture** — A Gated Recurrent Unit processes an IMU window $\{\tilde{\mathbf{a}}_t, \tilde{\boldsymbol{\omega}}_t\}_{t=1}^{T}$ sequentially:
$$\mathbf{z}_t = \sigma(W_z[\mathbf{h}_{t-1}, \mathbf{x}_t] + b_z) \quad \text{(update gate)}$$
$$\mathbf{r}_t = \sigma(W_r[\mathbf{h}_{t-1}, \mathbf{x}_t] + b_r) \quad \text{(reset gate)}$$
$$\tilde{\mathbf{h}}_t = \tanh(W_h[\mathbf{r}_t \odot \mathbf{h}_{t-1}, \mathbf{x}_t] + b_h)$$
$$\mathbf{h}_t = (1-\mathbf{z}_t)\odot\mathbf{h}_{t-1} + \mathbf{z}_t\odot\tilde{\mathbf{h}}_t$$
where $\mathbf{x}_t = [\tilde{\mathbf{a}}_t, \tilde{\boldsymbol{\omega}}_t] \in \mathbb{R}^6$, $\mathbf{h}_t \in \mathbb{R}^{512}$ (hidden state). The final hidden state is passed through an MLP head to predict displacement $\Delta\mathbf{p} \in \mathbb{R}^3$ and uncertainty $\Sigma_{\Delta p} \in \mathbb{R}^{3\times3}$.

3. **Output: displacement + covariance** — The network outputs:
   - $\hat{\Delta\mathbf{p}}$: predicted 3D displacement over window $T$
   - $\hat{\Sigma}$: predicted uncertainty (as $\log$-Cholesky factor to ensure PSD)
   The uncertainty is learned — the network can output larger uncertainty for high-vibration or ambiguous inputs, which classical preintegration cannot.

4. **Training objective** — Train with NLL loss on ground-truth displacement $\Delta\mathbf{p}^*$ (from IMU+vision ground truth or motion capture):
$$\mathcal{L} = \frac{1}{2}(\hat{\Delta\mathbf{p}} - \Delta\mathbf{p}^*)^\top\hat{\Sigma}^{-1}(\hat{\Delta\mathbf{p}} - \Delta\mathbf{p}^*) + \frac{1}{2}\log\det\hat{\Sigma}$$
The second term prevents the network from predicting infinite uncertainty. This is a self-calibrating loss — uncertainty is learned jointly with displacement.

5. **Why better than RK4** — Runge-Kutta 4th order is an accurate ODE integrator — it's not the ODE that's the problem, it's the **noise model and bias model**. GRU learns:
   - Implicit bias correction: hidden state $\mathbf{h}_t$ can track slowly-varying bias
   - Non-Gaussian noise: e.g. propeller vibration has structured frequency content → GRU can suppress it
   - Motion prior: learns that humans don't teleport → smooths physically unreasonable predictions
   Derive: classical preintegration is optimal under Gaussian noise + known bias. GRU is better when either assumption fails.

6. **Failure modes of GRU integrator** — Out-of-distribution motion (e.g. trained on walking, tested on running) → poor generalization. Requires large labeled IMU+GT dataset. Latency: RNN is sequential → not trivially parallelizable (but GRU window can be batched). Interpretability: classical preintegration has clear physical meaning; GRU is a black box.

7. **Integration into SLAM** — The GRU displacement prediction replaces the preintegration factor in the factor graph. The learned covariance $\hat{\Sigma}$ serves as the factor information matrix. Everything else (pose graph, BA, loop closure) remains unchanged.

---

## M29 — SVD: Derivation, Geometry, and Use in VINS-Fusion Initialization

### Problem Statement
Singular Value Decomposition is the core linear algebra tool in computer vision — used in PCA, DLT, homography estimation, triangulation, and VINS-Fusion's visual-inertial initialization. Derive it from scratch and show its role in VINS.

### Part 1: SVD Derivation

1. **Statement** — Every matrix $A \in \mathbb{R}^{m\times n}$ (with $m \geq n$) can be factored as:
$$A = U\Sigma V^\top$$
where $U \in \mathbb{R}^{m\times m}$ (orthogonal), $\Sigma \in \mathbb{R}^{m\times n}$ (diagonal, non-negative, decreasing), $V \in \mathbb{R}^{n\times n}$ (orthogonal). Columns of $U$ are **left singular vectors**, columns of $V$ are **right singular vectors**, diagonal of $\Sigma$ are **singular values** $\sigma_1 \geq \sigma_2 \geq \ldots \geq \sigma_n \geq 0$.

2. **Derivation via eigendecomposition** — $A^\top A = V\Sigma^\top\Sigma V^\top$ is an eigendecomposition of the symmetric PSD matrix $A^\top A$. Columns of $V$ are eigenvectors of $A^\top A$, $\sigma_i^2$ are eigenvalues. Left singular vectors: $\mathbf{u}_i = A\mathbf{v}_i/\sigma_i$. Derive this step-by-step.

3. **Geometric meaning** — SVD says $A$ maps the unit sphere to an ellipsoid. Right singular vectors $\mathbf{v}_i$ = input principal directions, left singular vectors $\mathbf{u}_i$ = output directions, singular values $\sigma_i$ = stretch factors. Draw the picture for $A\in\mathbb{R}^{2\times2}$.

4. **Low-rank approximation** — The best rank-$k$ approximation of $A$ (in Frobenius norm) is:
$$A_k = \sum_{i=1}^k \sigma_i \mathbf{u}_i \mathbf{v}_i^\top$$
Prove this (Eckart-Young theorem). Application: compress a $1000\times1000$ image matrix with $k=50$ singular values.

5. **Solving $A\mathbf{x}=\mathbf{0}$** — The solution is $\mathbf{x} = \mathbf{v}_n$ (last column of $V$), the right singular vector corresponding to $\sigma_n = \sigma_{min}$. Derive: $\|A\mathbf{x}\|^2 = \mathbf{x}^\top V\Sigma^2 V^\top \mathbf{x}$, minimized by $\mathbf{x} = \mathbf{v}_n$. This is the DLT solution (M14).

### Part 2: VINS-Fusion Visual-Inertial Initialization via SVD

6. **The initialization problem** — Before running the full nonlinear VIO optimization, VINS-Fusion needs good initial estimates of: velocity $\mathbf{v}_k$ for each keyframe, gravity vector $\mathbf{g}$ in body frame, scale $s$ (monocular case), and IMU biases. Without good initialization, the nonlinear solver diverges.

7. **Linear system setup** — Between keyframes $i$ and $i+1$, the IMU preintegration gives:
$$\Delta\mathbf{p}_{i,i+1} = R_i^\top(s(\mathbf{p}_{i+1} - \mathbf{p}_i) - \mathbf{v}_i\Delta t - \frac{1}{2}\mathbf{g}_w\Delta t^2)$$
$$\Delta\mathbf{v}_{i,i+1} = R_i^\top(\mathbf{v}_{i+1} - \mathbf{v}_i - \mathbf{g}_w\Delta t)$$
Rearrange to express as a linear system in the unknowns $[\mathbf{v}_0, \ldots, \mathbf{v}_K, \mathbf{g}_w, s]^\top$. Stack $K-1$ pairs → $A\mathbf{x} = \mathbf{b}$ where $A$ has structure from visual poses $\{R_i, \mathbf{p}_i\}$ (known from visual-only SfM) and IMU preintegrals.

8. **SVD solution** — Solve the overdetermined linear system via least squares: $\hat{\mathbf{x}} = (A^\top A)^{-1}A^\top\mathbf{b}$ or equivalently via thin SVD $A = U\Sigma V^\top$:
$$\hat{\mathbf{x}} = V\Sigma^{-1}U^\top\mathbf{b}$$
Extract $\hat{s}$ (scale), $\hat{\mathbf{g}}_w$ (gravity), $\{\hat{\mathbf{v}}_k\}$ (velocities). Explain why condition number of $A$ (ratio $\sigma_1/\sigma_n$) indicates how well-conditioned the initialization is — poor parallax or degenerate motion → large condition number → unreliable initialization.

9. **Gravity refinement** — The estimated $\hat{\mathbf{g}}_w$ should have magnitude $\approx 9.81\,\text{m/s}^2$. VINS-Fusion refines it by constraining $\|\mathbf{g}_w\| = g$ on a 2D tangent sphere: parameterize $\mathbf{g}_w = g(\hat{\mathbf{g}}_w + w_1\mathbf{b}_1 + w_2\mathbf{b}_2)/\|\cdot\|$ and solve for the 2D correction $[w_1,w_2]$ via another linear system.

10. **Why SVD over normal equations** — $A^\top A$ can be ill-conditioned (condition number squared). SVD directly gives the least-squares solution without squaring the condition number. For initialization where $A$ may be nearly rank-deficient (insufficient motion), SVD also reveals which components are unobservable (small singular values → corresponding $\mathbf{v}_i$ directions are poorly constrained).

---

## M30 — Solving Ax=b: Column Space, Row Space, Null Space, Least Squares

### Problem Statement
The system $A\mathbf{x} = \mathbf{b}$ underlies virtually every geometric estimation problem in robotics. Derive all cases — exact solution, homogeneous ($\mathbf{b}=\mathbf{0}$), overdetermined, underdetermined — in terms of the four fundamental subspaces.

### Tasks

1. **Four fundamental subspaces** — For $A \in \mathbb{R}^{m\times n}$ with rank $r$, derive and state the dimensions of:
   - Column space $\mathcal{C}(A) \subset \mathbb{R}^m$, dim $= r$
   - Row space $\mathcal{C}(A^\top) \subset \mathbb{R}^n$, dim $= r$
   - Null space $\mathcal{N}(A) \subset \mathbb{R}^n$, dim $= n - r$
   - Left null space $\mathcal{N}(A^\top) \subset \mathbb{R}^m$, dim $= m - r$
   State the orthogonality relationships: $\mathcal{C}(A^\top) \perp \mathcal{N}(A)$ and $\mathcal{C}(A) \perp \mathcal{N}(A^\top)$.

2. **Case 1: $A\mathbf{x} = \mathbf{b}$, $\mathbf{b} \in \mathcal{C}(A)$, square invertible** — Unique solution $\mathbf{x} = A^{-1}\mathbf{b}$. Derive existence condition: $\mathbf{b}$ must be in the column space of $A$.

3. **Case 2: Homogeneous $A\mathbf{x} = \mathbf{0}$** — Solution set = null space $\mathcal{N}(A)$. If $r = n$ (full column rank): only trivial solution $\mathbf{x} = \mathbf{0}$. If $r < n$: infinite solutions. For the unit-norm constrained problem $\min\|A\mathbf{x}\|$ s.t. $\|\mathbf{x}\|=1$: solution is the last right singular vector $\mathbf{v}_n$ of $A$ (from SVD). This is DLT (M14).

4. **Case 3: Overdetermined $m > n$, $\mathbf{b} \notin \mathcal{C}(A)$** — No exact solution. Minimize $\|A\mathbf{x} - \mathbf{b}\|^2$ → least squares. Decompose $\mathbf{b} = \mathbf{b}_{\mathcal{C}} + \mathbf{b}_{\mathcal{N}^\top}$ (projection onto $\mathcal{C}(A)$ and left null space). Derive normal equations:
$$A^\top A \mathbf{x} = A^\top \mathbf{b}$$
Solution: $\hat{\mathbf{x}} = (A^\top A)^{-1}A^\top\mathbf{b} = A^+\mathbf{b}$ (Moore-Penrose pseudoinverse). The residual $A\hat{\mathbf{x}} - \mathbf{b}$ lies in $\mathcal{N}(A^\top)$.

5. **Case 4: Underdetermined $m < n$** — Infinite solutions. Minimum norm solution: $\hat{\mathbf{x}} = A^\top(AA^\top)^{-1}\mathbf{b}$. Derive: this lies in the row space $\mathcal{C}(A^\top)$ — it has no null space component. The general solution is $\hat{\mathbf{x}} + \mathbf{x}_n$ for any $\mathbf{x}_n \in \mathcal{N}(A)$.

6. **Pseudoinverse via SVD** — $A^+ = V\Sigma^+U^\top$ where $\Sigma^+$ inverts non-zero singular values. Derive: for Case 3, $A^+\mathbf{b}$ gives the minimum-norm least-squares solution. Show how to detect rank deficiency: if $\sigma_r/\sigma_1 < \epsilon$, treat $\sigma_r$ as zero.

7. **Robotics examples** — Map each geometric estimation problem to a case:
   - DLT homography (8 eqs, 8 unknowns, noisy): Case 3 (overdetermined, least squares)
   - Triangulation (4 eqs, 4 unknowns homogeneous): Case 2 (homogeneous, SVD)
   - Underdetermined system in manifold optimization: Case 4
   - VINS initialization (M29): Case 3 with SVD

---

## M31 — RANSAC: Derivation, Sample Count, and Variants

### Problem Statement
RANSAC (Random Sample Consensus) is the standard outlier-rejection framework in computer vision. Derive the sample count formula from probability theory and formalize modern variants.

### Tasks

1. **The problem** — Given $n$ putative correspondences with unknown inlier ratio $\epsilon$ (fraction that are correct), fit a model (homography, Essential matrix, PnP) that is consistent with as many correspondences as possible. Brute force: $\binom{n}{s}$ subsets for a $s$-point minimal model — infeasible.

2. **RANSAC algorithm** — Repeat for $N$ iterations:
   - Sample $s$ correspondences uniformly at random (minimal sample)
   - Fit model $M$ to the sample (e.g. P3P → up to 4 poses)
   - Count inliers: $|\{i : d(x_i, M) < \tau\}|$ where $\tau$ is a threshold
   - Keep the model with the most inliers
   Final: re-fit the model to all inliers.

3. **Sample count derivation** — Probability that a single sample of $s$ points is all-inlier: $p_{good} = \epsilon^s$. Probability that at least one of $N$ samples is all-inlier: $p_{success} = 1 - (1-\epsilon^s)^N$. Solve for $N$ to achieve success probability $p$:
$$N = \frac{\log(1-p)}{\log(1-\epsilon^s)}$$
For $p=0.99$, $\epsilon=0.5$, $s=8$ (8-point F): $N = 1177$. For $s=3$ (P3P): $N = 35$. Derive: this is why P3P inside RANSAC is $34\times$ faster than 8-point F for the same inlier ratio.

4. **Inlier threshold $\tau$** — For pixel noise $\sigma$, the squared reprojection error follows $\chi^2(2)$ distribution for 2D residuals. Set $\tau^2 = \chi^2_{0.95}(2) \cdot \sigma^2 = 5.99\sigma^2$. Derive this. For $\sigma=1$ pixel: $\tau \approx 2.45$ pixels.

5. **Adaptive RANSAC** — Don't fix $N$ in advance. After each iteration, estimate $\hat{\epsilon}$ from the current best inlier set and update $N$:
$$N_{new} = \frac{\log(1-p)}{\log(1-\hat{\epsilon}^s)}$$
Terminate when the iteration count exceeds $N_{new}$. Derive why this reduces average iterations by $\sim3\times$ when the true inlier ratio is better than worst-case.

6. **MSAC (M-estimator SAC)** — Replace binary inlier/outlier scoring with a truncated M-estimator:
$$\text{score}(M) = \sum_i \min(d_i^2, \tau^2)$$
Inliers contribute $d_i^2$ (actual error), outliers contribute $\tau^2$ (a constant penalty). Derive why this gives more stable model estimates than binary RANSAC — inliers with smaller errors are weighted more.

7. **PROSAC (Progressive SAC)** — Sort correspondences by match quality score (e.g. descriptor distance ratio). Sample from the top-$m$ matches first, progressively including lower-quality matches. Derive: if inliers are concentrated in high-quality matches (which they often are), PROSAC finds a solution in $O(\log(1/\epsilon))$ samples instead of $O(1/\epsilon^s)$.

8. **MAGSAC (Sigma-Consensus)** — Marginalizes over all possible noise $\sigma$ values (assumes $\sigma$ unknown). Weights each correspondence by:
$$w_i = \int_0^\infty p(d_i|\sigma) p(\sigma) d\sigma$$
Derive that this is equivalent to a truncated Gaussian scoring, automatically adapting the threshold to the data. State-of-the-art on homography/F-matrix estimation benchmarks.

9. **RANSAC in a complete SLAM pipeline** — Map each use to a variant:
   - Feature matching + F/E matrix (loop closure): RANSAC or MAGSAC, $s=5$ or $s=8$
   - PnP in relocalization: P3P + Adaptive RANSAC, $s=3$
   - Homography for planar scene init: RANSAC, $s=4$
   - Why RANSAC is NOT used in the front-end LK tracking (M12): LK has forward-backward check instead (cheaper, works on consecutive frames)

---

## M33 — Find 3×3 Matrix A Such That Ax lies on line span{[2,1,2]} for all x

### Problem Statement
Find a $3\times3$ matrix $A$ such that $A\mathbf{x}$ always lies on the line $\text{span}\{[2,1,2]^\top\}$ for all $\mathbf{x} \in \mathbb{R}^3$. This means the column space of $A$ must equal $\text{span}\{[2,1,2]^\top\}$.

### Solution

$A$ must be a **rank-1 matrix** with all columns proportional to $\mathbf{b} = [2,1,2]^\top$.

General form via outer product:
$$A = \mathbf{b}\mathbf{c}^\top = [2,1,2]^\top [c_1, c_2, c_3]$$
for any nonzero $\mathbf{c} = [c_1, c_2, c_3]^\top$.

Simplest example ($\mathbf{c} = [1,0,0]^\top$):
$$A = \begin{bmatrix}2&0&0\\1&0&0\\2&0&0\end{bmatrix}$$

Then for any $\mathbf{x} = [x_1, x_2, x_3]^\top$:
$$A\mathbf{x} = x_1 \cdot [2,1,2]^\top$$

Always on the line. Column space = $\text{span}\{[2,1,2]^\top\}$. Rank = 1.

---

## M34 — Normal vs Gaussian Distribution: Are They the Same?

### Problem Statement
"Normal" and "Gaussian" are often used interchangeably but have distinct historical origins and subtle differences in usage. Formalize both, derive the multivariate generalization, and connect to the Central Limit Theorem.

### Tasks

1. **They are the same** — The normal distribution $\mathcal{N}(\mu, \sigma^2)$ and the Gaussian distribution refer to the same probability distribution:
$$f(x) = \frac{1}{\sigma\sqrt{2\pi}}\exp\left(-\frac{(x-\mu)^2}{2\sigma^2}\right)$$
**"Gaussian"** honors Carl Friedrich Gauss who used it to model astronomical measurement errors (1809). **"Normal"** was coined by Francis Galton/Karl Pearson (1880s) suggesting it was the "normal" distribution for natural phenomena. Both names are in use today — physicists and engineers often say Gaussian, statisticians often say normal.

2. **Standard normal** — $Z \sim \mathcal{N}(0,1)$. Derive that any $X \sim \mathcal{N}(\mu,\sigma^2)$ can be standardized: $Z = (X-\mu)/\sigma$. Derive the CDF $\Phi(z) = P(Z \leq z)$ has no closed form — define the error function $\text{erf}(x) = \frac{2}{\sqrt{\pi}}\int_0^x e^{-t^2}dt$ and show $\Phi(z) = \frac{1}{2}\left[1 + \text{erf}\left(\frac{z}{\sqrt{2}}\right)\right]$.

3. **Derive the PDF from first principles** — Gauss's original derivation: assume the MLE of the true value $\mu$ from measurements $\{x_i\}$ is their arithmetic mean $\bar{x}$. Show the only distribution with this property (and satisfying symmetry + independence) is the Gaussian. This is a derivation from axiomatic assumptions, not just a definition.

4. **Multivariate Gaussian** — For $\mathbf{x} \in \mathbb{R}^n$:
$$f(\mathbf{x}) = \frac{1}{(2\pi)^{n/2}|\Sigma|^{1/2}}\exp\left(-\frac{1}{2}(\mathbf{x}-\boldsymbol{\mu})^\top\Sigma^{-1}(\mathbf{x}-\boldsymbol{\mu})\right)$$
Derive: the exponent $(\mathbf{x}-\boldsymbol{\mu})^\top\Sigma^{-1}(\mathbf{x}-\boldsymbol{\mu})$ is the **Mahalanobis distance squared** — it measures distance in units of standard deviations accounting for correlation. Contours of constant density are ellipsoids aligned with the eigenvectors of $\Sigma$.

5. **Central Limit Theorem** — Why is the Gaussian so ubiquitous? If $X_1,\ldots,X_n$ are i.i.d. with mean $\mu$ and variance $\sigma^2$ (any distribution), then:
$$\frac{\bar{X}_n - \mu}{\sigma/\sqrt{n}} \xrightarrow{d} \mathcal{N}(0,1) \text{ as } n\to\infty$$
Prove using characteristic functions (moment generating functions): the CGF of the sum converges to the CGF of a Gaussian. This is why measurement noise (sum of many small independent errors) is Gaussian.

6. **In robotics** — The Gaussian assumption underlies every KF, EKF, UKF, and BA system. State when it fails: multimodal distributions (multiple hypothesis), heavy-tailed noise (outliers), bounded distributions (angles in $[0, 2\pi)$). Explain the von Mises distribution as the "Gaussian on the circle" for rotation uncertainty.

---

## M35 — Distribution of min(X, Y) When X, Y ~ Gaussian

### Problem Statement
If $X \sim \mathcal{N}(\mu_X, \sigma_X^2)$ and $Y \sim \mathcal{N}(\mu_Y, \sigma_Y^2)$, find the distribution of $M = \min(X, Y)$. This is an **order statistics** problem — the minimum does not follow a Gaussian, and the derivation reveals why.

### Tasks

### Part 1: General Framework — Order Statistics

1. **Setup** — For two random variables $X, Y$ (not necessarily independent), define $M = \min(X,Y)$. The CDF of $M$ satisfies:
$$F_M(m) = P(\min(X,Y) \leq m) = 1 - P(X > m, Y > m)$$

2. **Independent case** — When $X \perp Y$:
$$F_M(m) = 1 - P(X > m)\cdot P(Y > m) = 1 - (1 - F_X(m))(1 - F_Y(m))$$
Expand: $F_M(m) = F_X(m) + F_Y(m) - F_X(m)F_Y(m)$.

3. **PDF by differentiation**:
$$f_M(m) = f_X(m)(1 - F_Y(m)) + f_Y(m)(1 - F_X(m))$$
Interpret each term: $f_X(m)(1-F_Y(m))$ = probability $X$ is at $m$ AND $Y > m$ (so $X$ is the minimum).

### Part 2: When X, Y Are Gaussian

4. **Substituting** — Let $\Phi, \phi$ be the standard normal CDF and PDF. Write $F_X(m) = \Phi\left(\frac{m-\mu_X}{\sigma_X}\right)$ and so on. The PDF of $M$ is:
$$f_M(m) = \phi\left(\frac{m-\mu_X}{\sigma_X}\right)\frac{1}{\sigma_X}\left[1 - \Phi\left(\frac{m-\mu_Y}{\sigma_Y}\right)\right] + \phi\left(\frac{m-\mu_Y}{\sigma_Y}\right)\frac{1}{\sigma_Y}\left[1 - \Phi\left(\frac{m-\mu_X}{\sigma_X}\right)\right]$$

5. **Is $M$ Gaussian?** — No. The PDF is a **mixture of a Gaussian and a truncated Gaussian** term. It has no closed-form name. The distribution is:
   - Unimodal but **left-skewed** (the minimum tends to be pulled toward the smaller of $\mu_X, \mu_Y$)
   - Lighter right tail, heavier left tail than a Gaussian

6. **Special case: i.i.d.** — If $X, Y \sim \mathcal{N}(\mu, \sigma^2)$ i.i.d., then by symmetry:
$$f_M(m) = 2\phi\left(\frac{m-\mu}{\sigma}\right)\frac{1}{\sigma}\left[1 - \Phi\left(\frac{m-\mu}{\sigma}\right)\right]$$
This is a **lower-order statistic** of the normal distribution. Compute the mean:
$$\mathbb{E}[M] = \mu - \sigma\frac{1}{\sqrt{\pi}}$$
Derive this using the known result $\mathbb{E}[\min(Z_1,Z_2)] = -1/\sqrt{\pi}$ for i.i.d. $Z_i \sim \mathcal{N}(0,1)$.

7. **General $n$ variables** — For $n$ i.i.d. $\mathcal{N}(\mu,\sigma^2)$, the minimum $M_{(1)}$ has:
$$\mathbb{E}[M_{(1)}] = \mu - \sigma \cdot \mathbb{E}[Z_{(1:n)}]$$
where $\mathbb{E}[Z_{(1:n)}]$ is the expected minimum of $n$ standard normals — tabulated or approximated as $\approx -\Phi^{-1}\left(\frac{n}{n+1}\right)$. As $n\to\infty$, $M_{(1)} \to -\infty$ at rate $\sqrt{2\log n}$.

8. **Correlated case** — If $(X,Y)$ jointly Gaussian with correlation $\rho$:
$$F_M(m) = 1 - P(X>m, Y>m)$$
$P(X>m, Y>m)$ requires the **bivariate normal CDF** — no simple closed form. For $\rho=1$: $X=Y$ a.s. → $\min(X,Y) = X \sim \mathcal{N}(\mu,\sigma^2)$ (Gaussian). For $\rho=-1$: $X=-Y$ (if same mean/var) → $\min(X,Y)$ is concentrated at $\mu - \sigma|Z|$ (half-normal reflected).

9. **Robotics relevance** — The minimum of multiple sensor readings, minimum reprojection error across candidates, minimum path cost in motion planning — all involve order statistics. Knowing $\mathbb{E}[\min]$ is critical for:
   - Setting rejection thresholds in RANSAC (M31): the minimum error over random samples
   - Multi-hypothesis tracking: the weight of the best hypothesis
   - Nearest-neighbor matching: expected minimum descriptor distance to true match vs outlier

---

## M36 — Noodle Loop Probability: How Likely Is One Big Loop?

### Problem Statement
A bowl contains $n$ noodles (strands). Each noodle has two free ends, giving $2n$ ends total. You repeatedly pick two ends uniformly at random and tie them together, until all $2n$ ends are paired up. What is the probability that this process results in **one single large loop** (a Hamiltonian cycle through all noodles)?

This is a classic combinatorics/probability puzzle that appears in quant/SWE interviews and tests your ability to set up recursive or step-by-step probability arguments.

---

### Setup

- $n$ noodles → $2n$ free ends
- At each step, pick any two free ends at random and tie them
- Continue until no free ends remain
- **Goal:** $P(\text{one big loop})$

---

### Tasks

#### Part 1: Count the Total Number of Pairings

1. **Total pairings** — How many ways can $2n$ ends be paired into $n$ pairs (order within pairs and between pairs doesn't matter)?

$$\text{Total pairings} = (2n-1)!! = (2n-1)(2n-3)\cdots 3 \cdot 1 = \frac{(2n)!}{2^n \cdot n!}$$

Derive this: the first end has $2n-1$ choices for its partner, the next unpaired end has $2n-3$ choices, and so on.

#### Part 2: Count Pairings That Form One Big Loop

2. **Recursive count** — Let $L(n)$ = number of pairings of $2n$ ends that produce exactly one big loop. Argue:

$$L(n) = (2n-2) \cdot L(n-1), \quad L(1) = 1$$

**Argument:** Fix end $e_1$ (say, the left end of noodle 1). It must be connected to one of the $2n-2$ ends that are **not** the other end of noodle 1 (connecting to its own partner would close a small loop immediately). After tying $e_1$ to some end of another noodle, we have effectively merged two noodles into one longer strand, reducing the problem to $n-1$ noodles. By induction, all $n-1$ remaining strands must form one big loop.

Solve the recurrence:
$$L(n) = (2n-2)(2n-4)\cdots 2 = 2^{n-1}(n-1)!$$

#### Part 3: Derive the Probability

3. **Probability formula:**

$$\boxed{P(n) = \frac{L(n)}{(2n-1)!!} = \frac{2^{n-1}(n-1)!}{(2n-1)!!}}$$

**Equivalently** (by simplifying):

$$P(n) = \prod_{k=1}^{n-1} \frac{2k}{2k+1} = \frac{2}{3} \cdot \frac{4}{5} \cdot \frac{6}{7} \cdots \frac{2(n-1)}{2n-1}$$

#### Part 4: Step-by-Step Probability Argument (Alternative Derivation)

4. **Build the chain step by step** — This gives more intuition:

| Step $k$ | Free ends | Pick one chain-end; bad choice (premature close) | $P(\text{good})$ |
|----------|-----------|--------------------------------------------------|------------------|
| $k=1$ | $2n$ | 1 bad (own noodle's other end); $2n-2$ good out of $2n-1$ | $(2n-2)/(2n-1)$ |
| $k=2$ | $2n-2$ | 1 bad (close the chain early); $2n-4$ good out of $2n-3$ | $(2n-4)/(2n-3)$ |
| $\vdots$ | $\vdots$ | $\vdots$ | $\vdots$ |
| $k=n-1$ | $4$ | 1 bad out of $3$; $2$ good | $2/3$ |
| $k=n$ | $2$ | Must close the loop — only 1 choice, and it completes the big loop | $1$ |

Multiplying all factors:

$$P(n) = \frac{2n-2}{2n-1} \cdot \frac{2n-4}{2n-3} \cdots \frac{2}{3} = \prod_{j=1}^{n-1} \frac{2j}{2j+1}$$

This confirms the formula.

#### Part 5: Numerical Values and Asymptotics

5. **Compute for small $n$:**

| $n$ | $P(n)$ | Decimal |
|-----|--------|---------|
| 1 | $1$ | 1.000 |
| 2 | $2/3$ | 0.667 |
| 3 | $8/15$ | 0.533 |
| 4 | $16/35$ | 0.457 |
| 5 | $128/315$ | 0.406 |
| 10 | — | $\approx 0.273$ |

6. **Asymptotic behavior** — As $n \to \infty$, using the Wallis product or Stirling's approximation:

$$P(n) \sim \sqrt{\frac{\pi}{4n}} = \sqrt{\frac{\pi}{4n}}$$

More precisely, $P(n) = \Theta(n^{-1/2})$ — the probability decays to 0 but slowly (like $1/\sqrt{n}$).

**Derivation via Stirling:** Use $2^{n-1}(n-1)! \approx 2^{n-1}\sqrt{2\pi(n-1)}\left(\frac{n-1}{e}\right)^{n-1}$ and $(2n-1)!! = \frac{(2n)!}{2^n n!} \approx \frac{\sqrt{4\pi n}(2n/e)^{2n}}{2^n \cdot \sqrt{2\pi n}(n/e)^n}$.

#### Part 6: Verification for $n = 2$

7. **Enumerate for $n=2$ directly** — With 2 noodles $A$ and $B$, ends are $A_1, A_2, B_1, B_2$. Total pairings: $(4-1)!! = 3$:

| Pairing | Result |
|---------|--------|
| $\{A_1$-$A_2,\; B_1$-$B_2\}$ | Two small loops |
| $\{A_1$-$B_1,\; A_2$-$B_2\}$ | One big loop ✓ |
| $\{A_1$-$B_2,\; A_2$-$B_1\}$ | One big loop ✓ |

$P(2) = 2/3$. ✓

#### Part 7: Robotics/Systems Connection (Interview Angle)

8. **Why interviewers ask this** — Tests:
   - Ability to set up a recursive probability argument
   - Comfort with combinatorics (double factorials, product telescoping)
   - Knowing when to count vs. when to reason step-by-step
   - Asymptotic analysis

   The "avoid closing a premature loop" structure appears in:
   - **Cycle detection in random graphs** — Erdős–Rényi model, phase transitions
   - **Hash table analysis** — birthday paradox, collision chains
   - **Random permutations** — the number of cycles in a uniformly random permutation of $n$ elements has the same generating structure

### Key Results to Memorize

$$P(n) = \frac{2^{n-1}(n-1)!}{(2n-1)!!} = \prod_{k=1}^{n-1}\frac{2k}{2k+1} \approx \sqrt{\frac{\pi}{4n}}$$

- $P(1) = 1$, $P(2) = 2/3$, $P(3) = 8/15$
- Probability decreases monotonically with $n$, decays as $\sim 1/\sqrt{n}$
- At each step: exactly 1 out of the remaining odd number of choices is "bad" (closes the loop prematurely)

---

## M37 — SGBM: Semi-Global Block Matching

### Problem Statement
SGBM (Semi-Global Block Matching) is the dominant classical stereo depth algorithm — used in ROS `stereo_image_proc`, OpenCV, and nearly every production stereo camera pipeline. Understand how it works, what energy it minimizes, and where it breaks down.

---

### Part 1: Context — What Problem Does SGBM Solve?

Given a **rectified stereo pair** (left image $I_L$, right image $I_R$), find a **disparity map** $D$ where each pixel $p = (u, v)$ in $I_L$ is matched to pixel $(u - D(p), v)$ in $I_R$. Depth is then recovered as:

$$Z = \frac{f \cdot B}{D(p)}$$

where $f$ = focal length (pixels), $B$ = baseline (meters).

**Challenge:** Naive pixel-wise or block-wise matching is noisy in textureless regions and doesn't enforce spatial smoothness. Full 2D dynamic programming is NP-hard. SGBM approximates the global optimum by aggregating along multiple 1D paths.

---

### Part 2: The Energy Function SGBM Minimizes

SGBM minimizes a **Markov Random Field (MRF) energy**:

$$E(D) = \sum_{p} C(p, D_p) + \sum_{p} \sum_{q \in \mathcal{N}(p)} \left[ P_1 \cdot \mathbf{1}[|D_p - D_q| = 1] + P_2 \cdot \mathbf{1}[|D_p - D_q| > 1] \right]$$

**Terms:**
- $C(p, D_p)$: **data term** — matching cost at pixel $p$ for disparity $D_p$
- $P_1$: small penalty for disparity change of exactly 1 (allows slow-sloping surfaces)
- $P_2$: large penalty for disparity jumps $>1$ (discourages sharp discontinuities)
- $\mathcal{N}(p)$: 4-connected neighbors of $p$

**Parameters $P_1, P_2$:** Typically $P_2 > P_1 > 0$. In practice, $P_2$ is often **adaptive** — reduced at image edges (where real depth discontinuities occur):
$$P_2' = \max(P_2 / |I_L(p) - I_L(q)|, P_1)$$

---

### Part 3: Matching Cost $C(p, d)$

**Block Matching (the "B" in SGBM):** Instead of single-pixel cost, compute cost over a $W \times W$ window:

$$C(p, d) = \sum_{(i,j) \in \text{window}} \text{cost}(I_L(p + (i,j)),\ I_R(p + (i,j) - (d, 0)))$$

**Cost functions:**

| Cost | Formula | Properties |
|------|---------|-----------|
| SAD | $\sum |I_L - I_R|$ | Fast, sensitive to brightness |
| SSD | $\sum (I_L - I_R)^2$ | Penalizes large errors more |
| Census | Hamming distance of bit-strings | Illumination-invariant |
| BT (Birchfield-Tomasi) | Min interpolated SAD | Sub-pixel robust |

OpenCV's SGBM uses a modified **BT cost + Census transform**. The BT cost compares each pixel not to the exact integer location, but to the min over a small neighborhood, making it robust to sub-pixel shifts.

---

### Part 4: Semi-Global Aggregation

**Key idea:** Instead of 2D global optimization, aggregate costs along $r$ directions (typically 8 or 16 paths radiating from each pixel). For each path direction $\mathbf{r}$:

$$L_{\mathbf{r}}(p, d) = C(p, d) + \min \begin{cases} L_{\mathbf{r}}(p - \mathbf{r}, d) \\ L_{\mathbf{r}}(p - \mathbf{r}, d \pm 1) + P_1 \\ \min_{d'} L_{\mathbf{r}}(p - \mathbf{r}, d') + P_2 \end{cases} - \min_{d'} L_{\mathbf{r}}(p - \mathbf{r}, d')$$

The subtraction of $\min_{d'} L_{\mathbf{r}}(p - \mathbf{r}, d')$ prevents cost from growing unboundedly along the path.

**Aggregate all paths:**

$$S(p, d) = \sum_{\mathbf{r}} L_{\mathbf{r}}(p, d)$$

**Winner-Takes-All:**

$$D(p) = \arg\min_d S(p, d)$$

---

### Part 5: Post-Processing

1. **Left-Right consistency check:** Compute disparity from left ($D_L$) and from right ($D_R$). A pixel is valid if $|D_L(p) - D_R(p - D_L(p))| \leq 1$. Otherwise mark as invalid/occluded.

2. **Subpixel refinement:** Fit a parabola to $S(p, d-1), S(p, d), S(p, d+1)$ and find the sub-pixel minimum:
$$d^* = d - \frac{S(p,d+1) - S(p,d-1)}{2(S(p,d+1) - 2S(p,d) + S(p,d-1))}$$

3. **Speckle filtering:** Remove small connected components of disparity with similar values (noise artifacts in textureless regions).

4. **Optional median filter** to smooth remaining outliers.

---

### Part 6: Complexity

| Stage | Complexity |
|-------|-----------|
| Cost computation | $O(W^2 \cdot H \cdot D)$ per pixel; $O(N \cdot W^2 \cdot D)$ total |
| Path aggregation | $O(N \cdot r \cdot D)$ where $N = $ pixel count, $r = $ directions |
| WTA | $O(N \cdot D)$ |

$D$ = disparity range (e.g. 0–128 pixels). Full image: roughly $O(N \cdot r \cdot D)$.

---

### Part 7: Failure Modes

| Failure | Cause | Mitigation |
|---------|-------|-----------|
| Textureless regions | No discriminative cost signal | Larger window, regularization |
| Reflective/transparent surfaces | Violates Lambertian assumption | Domain-specific |
| Occlusions | Left-right inconsistency | L-R check + inpainting |
| Large disparities | Disparity range clipped | Increase `numDisparities` |
| Slanted surfaces | Block matching assumes fronto-parallel | Slant-support SGM |

---

### Part 8: SGM vs SGBM

| | SGM | SGBM |
|--|-----|------|
| Cost | Pixel-wise (single pixel) | Block/window cost |
| Robustness to noise | Lower | Higher (averaging over window) |
| Boundary sharpness | Sharper | Slightly blurred (window effect) |
| Speed | Faster | Slower (window computation) |

---

### Key Facts to Remember

- SGBM minimizes $E(D) = \text{data} + P_1 \cdot \text{small jumps} + P_2 \cdot \text{large jumps}$
- Aggregates along 8 directions to approximate 2D global optimization
- Data term uses block matching (window) — the "B" vs SGM's pixel-wise
- Post-processing: L-R check → subpixel → speckle filter
- Depth $Z = fB/D$; disparity precision → depth precision degrades quadratically with depth

---

## M38 — Epipolar Line Search: Geometry, Derivation, and Use in Stereo & SLAM

### Problem Statement
Given a point $\mathbf{x}$ in image 1, its correspondence in image 2 must lie on a specific line — the **epipolar line**. Derive where this constraint comes from, how to compute the epipolar line, and how stereo rectification collapses the search to a single row. This is foundational to SGBM, feature matching across views, and depth initialization in monocular SLAM.

---

### Part 1: Epipolar Geometry Setup

**Setup:** Two cameras with projection centers $O_1, O_2$. The **baseline** is the line $O_1 O_2$. The **epipoles** $e_1, e_2$ are the projections of $O_2$ into image 1 and $O_1$ into image 2.

A 3D point $\mathbf{P}$ lies on the plane $\pi$ defined by $O_1, O_2, \mathbf{P}$ (the **epipolar plane**). This plane intersects image 1 in a line through $e_1$ (epipolar line $l_1$) and image 2 in a line through $e_2$ (epipolar line $l_2$).

**Epipolar constraint:** The image points $\mathbf{x}_1, \mathbf{x}_2$ of $\mathbf{P}$ must each lie on their respective epipolar lines. This means: instead of a 2D search for $\mathbf{x}_2$ given $\mathbf{x}_1$, you only need a **1D search along the epipolar line** in image 2.

---

### Part 2: The Essential Matrix (Calibrated Cameras)

**Setup:** Cameras with known intrinsics $K_1, K_2$. Let $\mathbf{X}_1$ = bearing direction in camera 1's frame, $\mathbf{X}_2$ = bearing in camera 2's frame:

$$\mathbf{X}_1 = K_1^{-1} \tilde{\mathbf{x}}_1, \quad \mathbf{X}_2 = K_2^{-1} \tilde{\mathbf{x}}_2$$

Let $R, \mathbf{t}$ be the rotation and translation from camera 1 to camera 2. Then $\mathbf{P} = \lambda_1 \mathbf{X}_1$ and $\mathbf{P} = R\lambda_2 \mathbf{X}_2 + \mathbf{t}$.

**Derivation:** Cross both sides with $\mathbf{t}$:
$$\mathbf{t} \times \lambda_1 \mathbf{X}_1 = \mathbf{t} \times (R \lambda_2 \mathbf{X}_2 + \mathbf{t}) = \mathbf{t} \times R \lambda_2 \mathbf{X}_2$$

Take dot product with $\mathbf{X}_1$:
$$\mathbf{X}_1^\top (\mathbf{t} \times \mathbf{X}_1) = 0 = \mathbf{X}_1^\top [\mathbf{t}]_\times R \mathbf{X}_2$$

$$\boxed{\mathbf{X}_1^\top E \mathbf{X}_2 = 0}, \quad E = [\mathbf{t}]_\times R$$

$E \in \mathbb{R}^{3\times 3}$ is the **Essential Matrix** (rank 2, 5 DoF).

The epipolar line in image 2 (in normalized coordinates) is $\mathbf{l}_2 = E^\top \mathbf{X}_1$. In image 2, $\mathbf{X}_2$ must satisfy $\mathbf{l}_2^\top \mathbf{X}_2 = 0$.

---

### Part 3: The Fundamental Matrix (Uncalibrated Cameras)

Substituting $\mathbf{X}_i = K_i^{-1}\tilde{\mathbf{x}}_i$ into the essential matrix constraint:

$$\tilde{\mathbf{x}}_1^\top K_1^{-\top} E K_2^{-1} \tilde{\mathbf{x}}_2 = 0$$

$$\boxed{\tilde{\mathbf{x}}_1^\top F \tilde{\mathbf{x}}_2 = 0}, \quad F = K_1^{-\top} E K_2^{-1} = K_1^{-\top} [\mathbf{t}]_\times R K_2^{-1}$$

$F \in \mathbb{R}^{3\times 3}$ is the **Fundamental Matrix** (rank 2, 7 DoF — works with pixel coordinates, no need for $K$).

**Epipolar lines:**
$$\mathbf{l}_2 = F^\top \tilde{\mathbf{x}}_1 \quad \text{(line in image 2 where } \tilde{\mathbf{x}}_2 \text{ must lie)}$$
$$\mathbf{l}_1 = F \tilde{\mathbf{x}}_2 \quad \text{(line in image 1 where } \tilde{\mathbf{x}}_1 \text{ must lie)}$$

A line $\mathbf{l} = [a, b, c]^\top$ in homogeneous coordinates represents $au + bv + c = 0$.

---

### Part 4: Stereo Rectification — Collapsing 2D → 1D to a Row Search

**Goal:** Apply homographies $H_1, H_2$ to both images so that:
1. All epipolar lines become **horizontal** (same row in both images)
2. Corresponding points satisfy $u_2 = u_1 - d$ (disparity is purely horizontal)

**After rectification:** The search for the match of pixel $(u_1, v)$ in image 1 is reduced to scanning pixels $(u_1 - d, v)$ for $d \in [d_{\min}, d_{\max}]$ in image 2 — a pure 1D search along a scanline.

**How rectification is computed (Bouguet's method):**
1. Rotate both cameras by $R_1 = R^{1/2}$, $R_2 = R^{-1/2}$ so that the relative rotation between them becomes identity
2. Rotate both cameras so the baseline $\mathbf{t}$ aligns with the $x$-axis
3. Apply new intrinsics $K'$ (typically averaged focal length, same principal point)

**Result:** After rectification, the fundamental matrix becomes $F = [0, 0, 0; 0, 0, -1; 0, 1, 0]$ (epipolar lines are rows $v = \text{const}$).

---

### Part 5: Epipolar Line Search in SLAM (Monocular Depth Initialization)

In monocular SLAM (e.g., DSO, ORB-SLAM), when a new keyframe arrives and you want to **initialize the depth** of a feature seen in a previous keyframe:

1. **Known:** Feature pixel $\mathbf{x}_1$ in keyframe 1, relative pose $T_{21} = (R_{21}, \mathbf{t}_{21})$ to new keyframe 2.
2. **Compute epipolar line** in keyframe 2: $\mathbf{l}_2 = F^\top \tilde{\mathbf{x}}_1$ (or $K_2^{-\top} E^\top K_1^{-1} \tilde{\mathbf{x}}_1$ using calibrated form).
3. **Search along $\mathbf{l}_2$** for the best match (by photometric error or descriptor distance).
4. **Triangulate** the matched pair $(\mathbf{x}_1, \mathbf{x}_2)$ to get depth $Z$.

**Depth parameterization in the search:** The epipolar line has a **depth** parameterization — each point on $\mathbf{l}_2$ corresponds to a specific depth $Z$ for $\mathbf{x}_1$. As $Z \to \infty$, $\mathbf{x}_2$ approaches the epipole. This gives a non-uniform spacing in pixel space — why inverse depth search is preferred (uniform in $\rho = 1/Z$, see M2).

---

### Part 6: Epipolar Line Search Errors and Robustness

| Error source | Effect | Fix |
|---|---|---|
| Calibration error | Epipolar line is offset by $\epsilon$ pixels | Use wider search band ($\pm 2$ px) |
| Pose error (SLAM drift) | Epipolar line rotated/shifted | Verify with reprojection after triangulation |
| Textureless patches | No distinctive match along the line | Wait for better viewpoint, use semi-dense search |
| Repeated texture | Multiple low-cost matches | Use descriptor + geometry check |

**Sampson distance** — a first-order approximation to the geometric epipolar error:
$$d_{\text{Sampson}}(\mathbf{x}_1, \mathbf{x}_2) = \frac{(\tilde{\mathbf{x}}_2^\top F \tilde{\mathbf{x}}_1)^2}{(F\tilde{\mathbf{x}}_1)_1^2 + (F\tilde{\mathbf{x}}_1)_2^2 + (F^\top\tilde{\mathbf{x}}_2)_1^2 + (F^\top\tilde{\mathbf{x}}_2)_2^2}$$

Used in RANSAC (M31) to classify inliers when estimating $F$ or $E$.

---

### Part 7: Summary — Essential vs Fundamental vs Rectified

| | Essential $E$ | Fundamental $F$ | Rectified stereo |
|--|--|--|--|
| Works with | Normalized coords | Pixel coords | Pixel coords (after rectification) |
| Requires $K$? | Yes | No | Yes (for rectification) |
| DoF | 5 | 7 | 0 (fixed geometry) |
| Epipolar line | $\mathbf{l}_2 = E^\top \mathbf{X}_1$ | $\mathbf{l}_2 = F^\top \tilde{\mathbf{x}}_1$ | Horizontal scanline |
| Used in | SLAM (calibrated) | Structure from Motion | SGBM, SGM, block matching |
| Min correspondences to estimate | 5 | 7 (linear) / 8 | Calibration required |

---

### Key Results to Remember

- $F = K_1^{-\top}[\mathbf{t}]_\times R K_2^{-1}$, $E = [\mathbf{t}]_\times R$ — both rank 2
- Epipolar constraint: $\tilde{\mathbf{x}}_1^\top F \tilde{\mathbf{x}}_2 = 0$
- Epipolar line of $\tilde{\mathbf{x}}_1$ in image 2: $\mathbf{l}_2 = F^\top \tilde{\mathbf{x}}_1$
- Rectification makes epipolar lines horizontal → 1D disparity search
- In SLAM: search along epipolar line = initialize depth; use inverse depth for uniform sampling
- SGBM (M37) assumes rectified images — epipolar search is just a scanline scan

---

## Problem Index

| ID | Topic | Added | Done |
|----|-------|-------|------|
| M1 | Kalman Filter — position/velocity state derivation | Apr 3 | [x] |
| M2 | Inverse depth distribution — Gaussian depth → reciprocal normal | Apr 3 | [x] |
| M3 | IMU → Camera pose & covariance propagation | Apr 3 | [ ] |
| M4 | Pinhole camera projection equation — geometric derivation to full pipeline | Apr 3 | [x] |
| M5 | Camera distortion — full model, pipeline order, and Jacobian derivation | Apr 3 | [ ] |
| M6 | Kalman Filter vs nonlinear optimization — MAP root, EKF, factor graphs, sliding window | Apr 3 | [ ] |
| M7 | Gauss-Newton vs LM vs Dogleg — derivations, trust regions, failure modes, comparison | Apr 3 | [ ] |
| M8 | CLAHE — histogram equalization, clipping, illumination, and why it's used in VIO | Apr 3 | [ ] |
| M9 | GFTT vs FAST — corner detection derivations, scoring, speed trade-offs | Apr 3 | [ ] |
| M10 | SuperPoint vs GFTT — architecture, homographic adaptation, contrastive training, comparison | Apr 3 | [ ] |
| M11 | Descriptors, classical vs learned, LightGlue architecture, training, and why it wins | Apr 3 | [ ] |
| M12 | Lucas-Kanade optical flow — derivation, pyramid, vs descriptor matching | Apr 3 | [ ] |
| M13 | Essential & Fundamental matrix — derivation, epipolar geometry, depth from matches | Apr 3 | [ ] |
| M14 | DLT — Direct Linear Transform, homogeneous coordinates, SVD solution | Apr 3 | [ ] |
| M15 | PnP and variants — P3P, EPnP, DLT-PnP, homogeneous coordinates | Apr 3 | [ ] |
| M16 | Reprojection error vs photometric error — formalization, Jacobians, use cases | Apr 3 | [ ] |
| M17 | Covariance propagation in reprojection-based bundle adjustment | Apr 3 | [ ] |
| M18 | Monocular SLAM for a drone — optimization variables, factor graph, observability | Apr 3 | [ ] |
| M19 | Stereo depth estimation — disparity, triangulation, covariance propagation | Apr 3 | [ ] |
| M20 | FEJ (First Estimates Jacobian) — derivation with simple example, why it fixes inconsistency | Apr 3 | [ ] |
| M21 | Sliding window BA and marginalization — Schur complement, prior construction | Apr 3 | [ ] |
| M22 | Depth estimation in monocular SLAM — triangulation, multi-view stereo, depth filters | Apr 3 | [ ] |
| M23 | Covisibility graph — definition, construction, use in local BA and loop closure | Apr 3 | [ ] |
| M24 | Pose graph vs factor graph — formalization, nodes, edges, optimization, differences | Apr 3 | [ ] |
| M25 | iSAM / iSAM2 — incremental smoothing and mapping, Bayes tree, variable reordering | Apr 3 | [ ] |
| M26 | Loop closure — DBoW3, CosPlace, NetVLAD — place recognition pipeline and comparison | Apr 3 | [ ] |
| M27 | IMU preintegration — derivation, noise model, covariance propagation, bias correction | Apr 3 | [ ] |
| M28 | GRU/RNN as IMU integrator — architecture, training, why it beats classical ARK4 | Apr 3 | [ ] |
| M29 | SVD — derivation, geometric meaning, how VINS-Fusion uses it to initialize depth | Apr 3 | [ ] |
| M30 | Solving Ax=b — column/row/null spaces, homogeneous vs inhomogeneous, least squares | Apr 3 | [x] |
| M31 | RANSAC — derivation, sample count formula, variants (MSAC, PROSAC, MAGSAC) | Apr 3 | [ ] |
| M32 | Log-normal distribution — if Y~Gaussian, derive distribution of e^Y, moments, uses | Apr 3 | [ ] |
| M33 | Find 3×3 matrix A such that Ax lies on line span{[2,1,2]} for all x — rank-1, outer product | Apr 3 | [x] |
| M34 | Normal vs Gaussian distribution — are they the same? etymology, multivariate, CLT | Apr 3 | [ ] |
| M35 | Distribution of min(X,Y) when X,Y ~ Gaussian — order statistics, CDF derivation | Apr 3 | [ ] |
| M36 | Noodle loop probability — one big loop from n strands, recursive + step-by-step derivation | Apr 3 | [ ] |
| M37 | SGBM — energy function, block matching cost, semi-global path aggregation, post-processing | Apr 3 | [ ] |
| M38 | Epipolar line search — Essential/Fundamental matrix derivation, rectification, SLAM depth init | Apr 3 | [ ] |
| M39 | YOLO — architecture evolution (v1→v8), anchor boxes, loss function, NMS, deployment | Apr 3 | [ ] |
| M40 | TensorRT — inference optimization, layer fusion, quantization (FP16/INT8), engine building | Apr 3 | [ ] |
| M41 | ONNX — computational graph format, opset versioning, export/import workflow, optimizations | Apr 3 | [ ] |
| M42 | FAISS — approximate nearest neighbor search, index types, IVF/HNSW, GPU support, use cases | Apr 3 | [ ] |
| M43 | Why inverse depth formulation in VINS-Fusion? — motivation, numerical benefits, observability | Apr 3 | [ ] |
| M44 | Covariance propagation in a general factor graph and in a VIO sliding window — information matrix, marginalization, Schur complement | Apr 3 | [ ] |
| M45 | Eigen decomposition — intuitive explanation, geometric meaning, when it exists, relation to SVD | Apr 3 | [ ] |
| M46 | Lagrange multipliers — intuitive explanation with a concrete example, geometric meaning of the constraint | Apr 3 | [ ] |
| M47 | Lie group & Lie algebra — intuitive explanation with concrete SO(3)/se(3) example, exp/log maps, why used in SLAM | Apr 3 | [ ] |
| M48 | SVD — intuitive geometric explanation with a concrete example, singular values as stretching, relation to eigen decomposition | Apr 3 | [ ] |
| M49 | Monty Hall problem — full probability derivation, Bayes' theorem formulation, why intuition fails | Apr 3 | [ ] |
| M50 | Coupon collector problem — expected number of draws to collect all n coupons, derivation, variance | Apr 3 | [ ] |

---

## M39 — YOLO: You Only Look Once

### What Is YOLO?
YOLO is a family of **real-time object detection** models. Unlike two-stage detectors (e.g., Faster R-CNN) that first propose regions then classify them, YOLO is a **single-stage detector** — it predicts bounding boxes and class probabilities in one forward pass over the entire image.

**Core Idea:** Divide the image into an S×S grid. Each grid cell predicts B bounding boxes + confidence scores + C class probabilities — all simultaneously.

---

### Architecture Evolution

| Version | Year | Key Innovation |
|---------|------|----------------|
| YOLOv1  | 2016 | Single-stage detection, S×S grid, 7×7×30 output tensor |
| YOLOv2 (YOLO9000) | 2017 | Anchor boxes, Batch Norm, multi-scale training, Darknet-19 |
| YOLOv3  | 2018 | Multi-scale predictions (3 scales), Darknet-53 backbone, binary cross-entropy for classes |
| YOLOv4  | 2020 | CSPDarknet53, PANet neck, Mosaic augmentation, CIoU loss |
| YOLOv5  | 2020 | PyTorch rewrite, auto-anchor, Focus layer, strong community |
| YOLOv6  | 2022 | Meituan, anchor-free, RepVGG backbone, BiC-PAN neck |
| YOLOv7  | 2022 | E-ELAN, compound scaling, auxiliary heads |
| YOLOv8  | 2023 | Ultralytics, anchor-free, decoupled head, tasks: detect/seg/pose/classify |

---

### Key Concepts

#### 1. Grid Cell Prediction (YOLOv1 style)
- Image divided into S×S grid (e.g., 7×7)
- Each cell predicts B boxes: `(x, y, w, h, confidence)`
- Confidence = P(object) × IoU(pred, truth)
- Class probabilities C conditioned on object being present
- Output tensor: S × S × (B×5 + C)

#### 2. Anchor Boxes (YOLOv2+)
- Predefined bounding box shapes (aspect ratios) learned via k-means on training data
- Network predicts **offsets** relative to anchors, not absolute box coordinates
- Each grid cell predicts offsets for each anchor:
  - $b_x = \sigma(t_x) + c_x$, $b_y = \sigma(t_y) + c_y$
  - $b_w = p_w e^{t_w}$, $b_h = p_h e^{t_h}$
  - where $(c_x, c_y)$ = cell offset, $(p_w, p_h)$ = anchor size

#### 3. Multi-Scale Detection (YOLOv3+)
- Feature maps at 3 scales (e.g., 13×13, 26×26, 52×52)
- Large feature maps → detect small objects
- FPN/PANet neck fuses features across scales

#### 4. Anchor-Free Detection (YOLOv6/v7/v8)
- Predicts box center, width, height directly from feature map location
- No need to define anchors; simpler, faster to train
- Uses **Task-Aligned Assignment** (TAL) to match predictions to ground truth

---

### Loss Function

Total loss (YOLOv4-style):

$$\mathcal{L} = \lambda_{coord}\mathcal{L}_{box} + \mathcal{L}_{obj} + \mathcal{L}_{cls}$$

**Box loss:** CIoU (Complete IoU) — penalizes distance, aspect ratio, and overlap:
$$\mathcal{L}_{CIoU} = 1 - IoU + \frac{\rho^2(b, b^{gt})}{c^2} + \alpha v$$
- $\rho$ = Euclidean distance between centers
- $c$ = diagonal of enclosing box
- $v$ = aspect ratio consistency term, $\alpha$ = trade-off weight

**Objectness loss:** Binary cross-entropy on P(object)

**Class loss:** Binary cross-entropy per class (multi-label) or softmax (single-label)

---

### Non-Maximum Suppression (NMS)

After inference, many overlapping boxes are predicted. NMS removes redundant ones:

1. Sort all boxes by confidence score (descending)
2. Keep the highest-confidence box
3. Remove all boxes with IoU > threshold (e.g., 0.45) with the kept box
4. Repeat for remaining boxes

**Variants:**
- **Soft-NMS:** Instead of hard removal, reduce score by IoU overlap — better for dense scenes
- **DIoU-NMS:** Uses DIoU instead of IoU to consider center distance
- **NMS-free (YOLOv10):** Uses one-to-one matching head to avoid NMS at inference

---

### Deployment Pipeline

```
Training (PyTorch .pt)
       ↓
Export to ONNX (.onnx)
       ↓
Optimize with TensorRT (.engine)
       ↓
Inference on GPU (C++ / Python)
```

**YOLOv8 export:**
```python
from ultralytics import YOLO
model = YOLO("yolov8n.pt")
model.export(format="onnx")        # → yolov8n.onnx
model.export(format="engine")      # → yolov8n.engine (TensorRT)
```

---

### Key Interview Points
- YOLO trades accuracy for speed — excellent for real-time robotics
- Anchor-based: need tuning per dataset; anchor-free: simpler
- Multi-scale heads handle object size variation
- NMS is the main post-processing bottleneck; NMS-free models are emerging
- For deployment: always export ONNX → TensorRT for maximum GPU throughput

---

## M40 — TensorRT: High-Performance Inference

### What Is TensorRT?
NVIDIA TensorRT is an **SDK for high-performance deep learning inference**. It takes a trained model, optimizes it for the target GPU, and produces an **engine** that runs significantly faster than vanilla PyTorch/TensorFlow inference.

**Where it fits:**
```
Train (PyTorch/TF) → Export (ONNX) → Optimize (TensorRT) → Deploy (engine)
```

---

### Core Optimizations

#### 1. Layer Fusion
TensorRT fuses adjacent layers to reduce memory bandwidth and kernel launch overhead:
- **Vertical fusion:** Conv → BN → ReLU merged into one CUDA kernel
- **Horizontal fusion:** Multiple Conv layers with same input run in a single fused kernel
- Eliminates intermediate buffers, reduces GPU memory round-trips

#### 2. Precision Calibration (Quantization)

| Mode | Bits | Memory | Speed | Accuracy |
|------|------|--------|-------|----------|
| FP32 | 32   | Baseline | 1×  | Full |
| FP16 | 16   | 2× smaller | 2-3× | Near-lossless |
| INT8 | 8    | 4× smaller | 4-6× | Slight drop |

**INT8 Calibration:**  
INT8 maps float range $[-\alpha, \alpha]$ → $[-127, 127]$. TensorRT uses a **calibration dataset** to find the optimal $\alpha$ (saturation threshold) per layer using KL divergence minimization.

```python
# TensorRT INT8 calibration (pseudocode)
calibrator = trt.IInt8EntropyCalibrator2(calib_dataset, cache_file)
config.int8_calibrator = calibrator
config.set_flag(trt.BuilderFlag.INT8)
```

#### 3. Kernel Auto-Tuning
TensorRT benchmarks multiple CUDA kernel implementations for each op on your specific GPU and selects the fastest one. This is why building an engine takes time — it's doing actual profiling.

#### 4. Dynamic Shapes
- Static shapes: engine fixed at build time → fastest
- Dynamic shapes: engine handles variable batch/H/W at runtime using optimization profiles

```python
profile = builder.create_optimization_profile()
profile.set_shape("input", min=(1,3,320,320), opt=(1,3,640,640), max=(1,3,1280,1280))
config.add_optimization_profile(profile)
```

#### 5. Memory Management
- TensorRT pre-allocates all GPU memory at build time
- Uses workspace memory for intermediate computations
- Minimizes runtime allocations → predictable latency

---

### Build and Inference Workflow

```python
import tensorrt as trt
import numpy as np

# 1. Build engine from ONNX
logger = trt.Logger(trt.Logger.WARNING)
builder = trt.Builder(logger)
network = builder.create_network(1 << int(trt.NetworkDefinitionCreationFlag.EXPLICIT_BATCH))
parser = trt.OnnxParser(network, logger)

with open("model.onnx", "rb") as f:
    parser.parse(f.read())

config = builder.create_builder_config()
config.set_memory_pool_limit(trt.MemoryPoolType.WORKSPACE, 1 << 30)  # 1GB
config.set_flag(trt.BuilderFlag.FP16)

engine = builder.build_serialized_network(network, config)

# 2. Save engine
with open("model.engine", "wb") as f:
    f.write(engine)

# 3. Inference
runtime = trt.Runtime(logger)
engine = runtime.deserialize_cuda_engine(open("model.engine", "rb").read())
context = engine.create_execution_context()
# allocate buffers, run context.execute_async_v2(...)
```

---

### Key Concepts for Robotics

- **Jetson platforms** (AGX Orin, Xavier, Nano): TensorRT is the primary inference path
- Engine is **GPU-specific** — cannot transfer engine file between different GPU architectures
- **Plugin API:** Custom CUDA ops can be registered as TensorRT plugins for unsupported layers
- **TensorRT + CUDA streams:** Enables overlap of inference, pre/post-processing for max throughput
- **Latency vs Throughput:** Batch size 1 → minimize latency; large batch → maximize throughput

---

### Typical Speedups (YOLOv8n, Jetson AGX Orin)

| Mode | Latency |
|------|---------|
| PyTorch FP32 | ~25 ms |
| TensorRT FP32 | ~8 ms |
| TensorRT FP16 | ~4 ms |
| TensorRT INT8 | ~2.5 ms |

---

### Key Interview Points
- TensorRT doesn't retrain — it optimizes the inference graph
- FP16 is almost always safe; INT8 needs calibration data
- Engine is not portable across GPU generations
- Layer fusion is the biggest win for CNN-heavy models
- For YOLO: Conv+BN+SiLU gets fused into one kernel

---

## M41 — ONNX: Open Neural Network Exchange

### What Is ONNX?
ONNX is an **open standard for representing ML models as a computational graph**. It acts as the universal interchange format between training frameworks (PyTorch, TensorFlow, JAX) and inference runtimes (TensorRT, OpenVINO, ONNX Runtime, CoreML).

**Problem it solves:** Each framework has its own model format. ONNX provides a common IR (Intermediate Representation) so you train once and deploy anywhere.

---

### ONNX Graph Structure

An ONNX model is a **directed acyclic graph (DAG)**:

```
ModelProto
├── graph: GraphProto
│   ├── node[]: NodeProto       ← operations (Conv, Relu, Gemm, ...)
│   ├── input[]: ValueInfoProto ← input tensors + shapes/types
│   ├── output[]: ValueInfoProto← output tensors
│   ├── initializer[]: TensorProto ← weights (constants)
│   └── value_info[]: ValueInfoProto ← intermediate tensor shapes
├── opset_import[]              ← which opset version
└── ir_version                  ← ONNX IR version
```

Each **NodeProto** has:
- `op_type`: operation name (e.g., "Conv", "BatchNormalization", "Reshape")
- `inputs[]`: names of input tensors
- `outputs[]`: names of output tensors
- `attribute[]`: hyperparameters (kernel size, strides, etc.)

---

### Opset Versioning
ONNX operators are versioned. Each opset version may change operator semantics:
- Opset 11: `Resize` operator semantics changed
- Opset 13: `Softmax` axis default changed
- Opset 17: `LayerNormalization` added

Always specify opset when exporting:
```python
torch.onnx.export(model, dummy_input, "model.onnx", opset_version=17)
```

**Rule:** Use the highest opset supported by your target runtime.

---

### Export Workflow (PyTorch → ONNX)

```python
import torch
import torch.onnx

model = MyModel().eval()
dummy_input = torch.randn(1, 3, 640, 640)

torch.onnx.export(
    model,
    dummy_input,
    "model.onnx",
    input_names=["images"],
    output_names=["output"],
    dynamic_axes={"images": {0: "batch"}, "output": {0: "batch"}},
    opset_version=17,
    do_constant_folding=True,   # fold constant subgraphs
)
```

**Dynamic axes:** Marks certain dimensions as variable (batch size, image size). Without this, all shapes are fixed (static).

---

### ONNX Graph Optimizations

Tools like `onnxoptimizer` and `onnxsim` apply passes:

| Pass | What It Does |
|------|-------------|
| Constant folding | Pre-compute ops with all-constant inputs |
| Eliminate identity | Remove no-op Identity nodes |
| Fuse BN into Conv | Merge BatchNorm weights into preceding Conv |
| Eliminate unused nodes | Dead code elimination |
| Reshape elimination | Remove redundant Reshape/Transpose pairs |

```bash
python -m onnxsim model.onnx model_simplified.onnx
```

---

### ONNX Runtime (ORT)

Microsoft's inference engine for ONNX models:
```python
import onnxruntime as ort
import numpy as np

sess = ort.InferenceSession("model.onnx", providers=["CUDAExecutionProvider"])
inputs = {"images": np.random.randn(1, 3, 640, 640).astype(np.float32)}
outputs = sess.run(None, inputs)
```

**Execution Providers:** CPU, CUDA, TensorRT, OpenVINO, CoreML, ROCm — ORT selects the best available.

---

### ONNX → TensorRT Pipeline

```
PyTorch .pt
    ↓  torch.onnx.export()
ONNX .onnx
    ↓  onnxsim (optional simplification)
Simplified ONNX
    ↓  trtexec or TensorRT Python API
TensorRT .engine
    ↓  context.execute_async_v2()
GPU Inference
```

```bash
# trtexec command-line build
trtexec --onnx=model.onnx --saveEngine=model.engine --fp16
```

---

### Common Issues and Fixes

| Problem | Cause | Fix |
|---------|-------|-----|
| Unsupported op | Op not in ONNX standard | Use `torch.onnx.register_custom_op` or decompose |
| Shape mismatch | Dynamic shapes not declared | Add `dynamic_axes` in export |
| Opset mismatch | Runtime supports lower opset | Lower `opset_version` in export |
| Large file | Weights embedded in graph | Use external data: `save_as_external_data=True` |

---

### Key Interview Points
- ONNX is the de facto standard for model portability in production
- Static vs dynamic shapes: static is faster but inflexible
- Always simplify ONNX before TensorRT conversion — fewer nodes = faster build
- `do_constant_folding=True` is almost always correct
- Check opset compatibility matrix for your target runtime

---

## M42 — FAISS: Facebook AI Similarity Search

### What Is FAISS?
FAISS is a library by Meta AI for **efficient similarity search and clustering of dense vectors**. Given a database of N vectors and a query vector, FAISS finds the k nearest neighbors (by L2 or inner product) — even when N is in the billions.

**Why it matters for robotics:** Place recognition (NetVLAD, CosPlace), relocalization, loop closure — all require fast nearest-neighbor search in high-dimensional descriptor spaces.

---

### The Core Problem

Given:
- Database: $X \in \mathbb{R}^{N \times d}$ (N vectors, d dimensions)
- Query: $q \in \mathbb{R}^d$
- Find: $k$ vectors in $X$ with smallest $\|q - x_i\|^2$

**Brute force:** $O(Nd)$ per query — fine for small N, prohibitive for $N > 10^6$.

---

### Index Types

#### 1. Flat Index (Exact Search)
```python
index = faiss.IndexFlatL2(d)   # exact L2 search
index.add(xb)                  # add database vectors
D, I = index.search(xq, k)    # D=distances, I=indices
```
- No compression, no approximation
- Complexity: $O(Nd)$ per query
- Use when N < 100K and accuracy is critical

#### 2. IVF (Inverted File Index) — Approximate
**Idea:** Cluster vectors into $n_{list}$ Voronoi cells using k-means. At query time, only search $n_{probe}$ nearest cells.

```python
quantizer = faiss.IndexFlatL2(d)
index = faiss.IndexIVFFlat(quantizer, d, nlist=100)
index.train(xb)                 # k-means clustering required
index.add(xb)
index.nprobe = 10               # search 10 cells (trade-off: speed vs recall)
D, I = index.search(xq, k)
```

- Training required (k-means)
- $n_{probe}$ controls speed vs recall: higher = slower but more accurate
- Complexity: $O(n_{probe} \cdot N/n_{list} \cdot d)$

#### 3. HNSW (Hierarchical Navigable Small World) — Approximate
Graph-based index: vectors are nodes, edges connect nearby vectors in a hierarchical graph.

```python
index = faiss.IndexHNSWFlat(d, M=32)  # M = edges per node
index.hnsw.efConstruction = 40        # build quality
index.add(xb)
index.hnsw.efSearch = 64              # search quality
D, I = index.search(xq, k)
```

- No training required
- Very fast query; slower to build; high memory (graph edges)
- State-of-the-art recall/speed tradeoff for RAM-based search

#### 4. IVF + PQ (Product Quantization) — Compressed
**Product Quantization:** Splits each vector into $m$ sub-vectors, quantizes each independently. Stores compressed codes, not full vectors.

```python
index = faiss.IndexIVFPQ(quantizer, d, nlist=100, m=8, nbits=8)
# d/m = sub-vector size, 2^nbits = centroids per sub-space
index.train(xb)
index.add(xb)
```

- Drastically reduces memory: $d \times 4$ bytes → $m$ bytes per vector
- Approximate distances via lookup tables
- Tradeoff: speed and memory for recall

---

### Similarity Metrics

```python
faiss.IndexFlatL2(d)           # Euclidean (L2) distance
faiss.IndexFlatIP(d)           # Inner product (cosine if normalized)

# For cosine similarity: normalize vectors first
faiss.normalize_L2(xb)
faiss.normalize_L2(xq)
index = faiss.IndexFlatIP(d)
```

---

### GPU Support

FAISS can move indexes to GPU for massive speedup:

```python
res = faiss.StandardGpuResources()
index_gpu = faiss.index_cpu_to_gpu(res, 0, index)  # move to GPU 0
D, I = index_gpu.search(xq, k)
```

- GPU Flat: 10-100× speedup over CPU
- GPU IVF: scales well with large N

---

### Choosing the Right Index

| Scenario | Recommended Index | Why |
|----------|------------------|-----|
| N < 100K, need exact | IndexFlatL2 | Simple, exact |
| N ~ 1M, RAM OK | IndexHNSWFlat | Best recall/speed |
| N ~ 1M, build speed matters | IndexIVFFlat | Faster build than HNSW |
| N > 10M, memory limited | IndexIVFPQ | Compressed storage |
| Real-time on GPU | GpuIndexFlat | Maximum throughput |

---

### Robotics / SLAM Use Cases

- **Loop closure:** Store global descriptors (NetVLAD, CosPlace) of keyframes. Query with current frame descriptor → find nearest keyframe → close loop.
- **Place recognition:** Retrieve top-k candidate places from a map database.
- **Relocalization:** Match current frame descriptors against a map built offline.

**Typical pipeline:**
```python
# Offline: build map descriptor database
index = faiss.IndexHNSWFlat(512, 32)
index.add(map_descriptors)    # (N, 512) float32
faiss.write_index(index, "map.faiss")

# Online: relocalize
index = faiss.read_index("map.faiss")
D, I = index.search(query_descriptor, k=5)
candidate_keyframes = keyframe_list[I[0]]
```

---

### Key Interview Points
- FAISS is for dense vector search — not for sparse (use inverted index for that)
- IVF requires training; HNSW does not
- PQ trades memory for recall — crucial for billion-scale datasets
- Always normalize vectors before using IndexFlatIP for cosine similarity
- GPU indexes require fitting data in GPU VRAM
- `nprobe` (IVF) and `efSearch` (HNSW) are the main runtime accuracy knobs

---

## M43 — Why Inverse Depth Formulation in VINS-Fusion?

### Question
Why does VINS-Fusion (and most monocular VIO systems) parameterize 3D points using **inverse depth** rather than Euclidean depth or 3D coordinates? What are the numerical, statistical, and observability motivations?

---

## M44 — Covariance Propagation in a Factor Graph and VIO Sliding Window

### Question
How does covariance propagate through a general factor graph? Then specifically for a VIO sliding window: when you marginalize out old states, how is the prior covariance constructed, what role does the Schur complement play, and how does the information matrix relate to the covariance of the remaining variables?

---

## M45 — Eigen Decomposition: Intuitive Explanation

### Question
What is eigen decomposition intuitively — what do eigenvalues and eigenvectors actually represent geometrically? When does it exist, when is it unique, and how does it relate to SVD?

---

## M46 — Lagrange Multipliers: Intuitive Explanation

### Question
What is the intuition behind Lagrange multipliers? Walk through a concrete example (e.g. minimize $f(x,y)$ subject to $g(x,y) = 0$) — why must the gradients be parallel at the optimum, and what does $\lambda$ represent geometrically?

---

## M47 — Lie Group & Lie Algebra: Intuitive Explanation

### Question
What is a Lie group and its associated Lie algebra intuitively? Using SO(3) and se(3) as concrete examples — what do the exp and log maps do geometrically, why can't you just add rotation matrices, and why does SLAM optimization live in the Lie algebra rather than directly on the manifold?

---

## M48 — SVD: Intuitive Explanation

### Question
What does SVD ($A = U\Sigma V^T$) mean geometrically with a concrete example — what do $U$, $\Sigma$, $V$ each do to a vector? How do singular values relate to eigenvalues, and why is SVD more general than eigen decomposition?

---

## M49 — Monty Hall Problem

### Question
You pick one of 3 doors. The host opens a different door revealing a goat. Should you switch? Derive the exact probabilities using both conditional probability and Bayes' theorem, and explain why the answer contradicts most people's intuition.

---

## M50 — Coupon Collector Problem

### Question
There are $n$ distinct coupons. Each draw gives one uniformly at random (with replacement). What is the expected number of draws to collect all $n$ coupons? Derive $E[T]$, find the variance, and give the asymptotic form.
