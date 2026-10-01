# Static evidence — a9da810a verification (generated 2026-10-01)

Tree: HEAD ce36144e + pre-registration commit. Source of the pinned exe
(SHA-256 9FF13F2797C95281107DCE737739EE6E9B48926B61570289F721F490D986E5F9).

## T5a — every assignment to a CamInput field in exe_main.cpp
```
2887:        ci.dt = dt;
```
Exactly one, and it is the frame delta. No motion field is ever written.

## T5a — removed free-fly identifiers in exe_main.cpp (expect: no output)
```
(no matches)
```

## T5a — consumer gate, D3d9Render/TrackRenderer.cpp
```
        if (in) {
            const bool wants_free =
                in->move_fwd != 0.f || in->move_strafe != 0.f ||
                in->move_up != 0.f || in->yaw_delta != 0.f || in->pitch_delta != 0.f;
            if (in->reset_orbit) free_ = false;
            if (wants_free && !free_) {
                // seed the free camera from the current orbit pose
                const float yaw0 = t * 0.3f;
                eye_[0] = center_[0] + radius_ * 1.15f * std::cos(yaw0);
                eye_[1] = center_[1] + radius_ * 0.55f;
                eye_[2] = center_[2] + radius_ * 1.15f * std::sin(yaw0);
                yaw_   = std::atan2(center_[2] - eye_[2], center_[0] - eye_[0]);
                pitch_ = -0.4f;
                free_  = true;
            }
            if (free_) {
                yaw_   += in->yaw_delta;
--- (lines 5174-5190 above; line 5497 below) ---
        const bool chase_cam = car_ready_ && !free_;
```
free_ is assigned true only inside the wants_free branch, and wants_free
requires a nonzero motion field. With CamInput all-zero, free_ is
unreachable, so chase_cam == car_ready_ and the race camera drives the view.

## T6c — B11/B12 identifiers in exe_main.cpp (expect: no output)
```
(no matches)
```

## T6a — arrow-key drive mapping, still present
```
                } else if (g_kbd && !g_det_clock && s_live_input_ok) {   // R10b/#6: no ambient/unfocused steering
                    auto dn = [&](int k) { return (g_keys[k] & 0x80) != 0; };
                    di.accel = (dn(DIK_UP) ? 1.f : 0.f) - (dn(DIK_DOWN) ? 1.f : 0.f);
                    di.steer = (dn(DIK_RIGHT) ? 1.f : 0.f) - (dn(DIK_LEFT) ? 1.f : 0.f);
                }
```
a9da810a shows these lines as unchanged CONTEXT (leading space) in its diff,
i.e. the commit did not touch the arrow-to-car mapping:
```
256:                 di.accel = (dn(DIK_UP) ? 1.f : 0.f) - (dn(DIK_DOWN) ? 1.f : 0.f);
257:                 di.steer = (dn(DIK_RIGHT) ? 1.f : 0.f) - (dn(DIK_LEFT) ? 1.f : 0.f);
```
