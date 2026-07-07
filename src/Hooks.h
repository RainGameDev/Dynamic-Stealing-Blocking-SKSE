#pragma once

namespace Hooks {
    // Swaps ActivateHandler::ProcessButton (vtable) so the activate action is
    // suppressed when taking the crosshair target would be a theft, the player is
    // detected, and Shift is not held. Call once (kDataLoaded).
    void Install();
}
