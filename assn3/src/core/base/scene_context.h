#pragma once

#include "core/base/scene_node.h"

inline SceneNode* gSceneRoot = nullptr;

inline void set_scene_root(SceneNode* root) { gSceneRoot = root; }
inline SceneNode* get_scene_root() { return gSceneRoot; }
