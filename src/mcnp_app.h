#ifndef MCNP_APP_H
#define MCNP_APP_H

// MCNP 3D建模应用程序主头文件
// 包含所有核心模块的头文件

// Core 模块
#include "core/vertex_mesh.h"
#include "core/geometry_factory.h"
#include "core/log_manager.h"
#include "core/coordinate_system.h"

// IO 模块
#include "io/config_manager.h"
#include "io/scene_manager.h"
#include "io/command_parser.h"

// UI 模块
#include "ui/ui_interface.h"
#include "ui/input_control.h"
#include "ui/transform_controller.h"

// Render 模块
#include "render/render.h"

// 该头文件提供了一站式访问所有模块的功能
// 用户只需包含此头文件即可访问所有功能

#endif // MCNP_APP_H