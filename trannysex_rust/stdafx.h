#include <windows.h>
#include <winternl.h>
#include <d3dcompiler.h>
#include <iostream>
#include <dwmapi.h>
#include <intrin.h>
#include <d3d11.h>
#include <psapi.h>

//#define prints
#ifdef prints
#define print macro::print_message
#else
#define print //
#endif

extern bool setup_complete;

#include "xor.h"
#include "vector.h"
#include "memory.h"
#include "linked_list.h"
#include "offsets.h"
#include "undocumented.h"
#include "macro.h"
#include "math.h"
#include "misc_classes.h"
#include "vec3.h"

#include "overlay.h"
#include "drawings.h"
#include "settings.h"
#include "input.h"
#include "holy_font.h"
#include "entities.h"
#include "visuals.h"
#include "util.h"
#include "menu.h"
#include "aim.h"