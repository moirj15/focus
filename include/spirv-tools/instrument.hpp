// Copyright (c) 2018 The Khronos Group Inc.
// Copyright (c) 2018 Valve Corporation
// Copyright (c) 2018 LunarG Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDE_SPIRV_TOOLS_INSTRUMENT_HPP_
#define INCLUDE_SPIRV_TOOLS_INSTRUMENT_HPP_

// Shader Instrumentation Interface
//
// This file provides an external interface for applications that wish to
// communicate with shaders instrumented by passes created by:
//
//   CreateInstBindlessCheckPass
//   CreateInstBuffAddrCheckPass
//
// More detailed documentation of these routines can be found in optimizer.hpp

namespace spvtools {

// Stream Output Buffer Offsets
//
// The following values provide offsets into the output buffer struct
// generated by InstrumentPass::GenDebugStreamWrite. This method is utilized
// by InstBindlessCheckPass.
//
// The first member of the debug output buffer contains the next available word
// in the data stream to be written. Shaders will atomically read and update
// this value so as not to overwrite each others records. This value must be
// initialized to zero
static const int kDebugOutputSizeOffset = 0;

// The second member of the output buffer is the start of the stream of records
// written by the instrumented shaders. Each record represents a validation
// error. The format of the records is documented below.
static const int kDebugOutputDataOffset = 1;

// Common Stream Record Offsets
//
// The following are offsets to fields which are common to all records written
// to the output stream.
//
// Each record first contains the size of the record in 32-bit words, including
// the size word.
static const int kInstCommonOutSize = 0;

// This is the shader id passed by the layer when the instrumentation pass is
// created.
static const int kInstCommonOutShaderId = 1;

// This is the ordinal position of the instruction within the SPIR-V shader
// which generated the validation error.
static const int kInstCommonOutInstructionIdx = 2;

// This is the stage which generated the validation error. This word is used
// to determine the contents of the next two words in the record.
// 0:Vert, 1:TessCtrl, 2:TessEval, 3:Geom, 4:Frag, 5:Compute
static const int kInstCommonOutStageIdx = 3;
static const int kInstCommonOutCnt = 4;

// Stage-specific Stream Record Offsets
//
// Each stage will contain different values in the next set of words of the
// record used to identify which instantiation of the shader generated the
// validation error.
//
// Vertex Shader Output Record Offsets
static const int kInstVertOutVertexIndex = kInstCommonOutCnt;
static const int kInstVertOutInstanceIndex = kInstCommonOutCnt + 1;
static const int kInstVertOutUnused = kInstCommonOutCnt + 2;

// Frag Shader Output Record Offsets
static const int kInstFragOutFragCoordX = kInstCommonOutCnt;
static const int kInstFragOutFragCoordY = kInstCommonOutCnt + 1;
static const int kInstFragOutUnused = kInstCommonOutCnt + 2;

// Compute Shader Output Record Offsets
static const int kInstCompOutGlobalInvocationIdX = kInstCommonOutCnt;
static const int kInstCompOutGlobalInvocationIdY = kInstCommonOutCnt + 1;
static const int kInstCompOutGlobalInvocationIdZ = kInstCommonOutCnt + 2;

// Tessellation Control Shader Output Record Offsets
static const int kInstTessCtlOutInvocationId = kInstCommonOutCnt;
static const int kInstTessCtlOutPrimitiveId = kInstCommonOutCnt + 1;
static const int kInstTessCtlOutUnused = kInstCommonOutCnt + 2;

// Tessellation Eval Shader Output Record Offsets
static const int kInstTessEvalOutPrimitiveId = kInstCommonOutCnt;
static const int kInstTessEvalOutTessCoordU = kInstCommonOutCnt + 1;
static const int kInstTessEvalOutTessCoordV = kInstCommonOutCnt + 2;

// Geometry Shader Output Record Offsets
static const int kInstGeomOutPrimitiveId = kInstCommonOutCnt;
static const int kInstGeomOutInvocationId = kInstCommonOutCnt + 1;
static const int kInstGeomOutUnused = kInstCommonOutCnt + 2;

// Ray Tracing Shader Output Record Offsets
static const int kInstRayTracingOutLaunchIdX = kInstCommonOutCnt;
static const int kInstRayTracingOutLaunchIdY = kInstCommonOutCnt + 1;
static const int kInstRayTracingOutLaunchIdZ = kInstCommonOutCnt + 2;

// Size of Common and Stage-specific Members
static const int kInstStageOutCnt = kInstCommonOutCnt + 3;

// Validation Error Code Offset
//
// This identifies the validation error. It also helps to identify
// how many words follow in the record and their meaning.
static const int kInstValidationOutError = kInstStageOutCnt;

// Validation-specific Output Record Offsets
//
// Each different validation will generate a potentially different
// number of words at the end of the record giving more specifics
// about the validation error.
//
// A bindless bounds error will output the index and the bound.
static const int kInstBindlessBoundsOutDescIndex = kInstStageOutCnt + 1;
static const int kInstBindlessBoundsOutDescBound = kInstStageOutCnt + 2;
static const int kInstBindlessBoundsOutCnt = kInstStageOutCnt + 3;

// A bindless uninitialized error will output the index.
static const int kInstBindlessUninitOutDescIndex = kInstStageOutCnt + 1;
static const int kInstBindlessUninitOutUnused = kInstStageOutCnt + 2;
static const int kInstBindlessUninitOutCnt = kInstStageOutCnt + 3;

// A buffer address unalloc error will output the 64-bit pointer in
// two 32-bit pieces, lower bits first.
static const int kInstBuffAddrUnallocOutDescPtrLo = kInstStageOutCnt + 1;
static const int kInstBuffAddrUnallocOutDescPtrHi = kInstStageOutCnt + 2;
static const int kInstBuffAddrUnallocOutCnt = kInstStageOutCnt + 3;

// Maximum Output Record Member Count
static const int kInstMaxOutCnt = kInstStageOutCnt + 3;

// Validation Error Codes
//
// These are the possible validation error codes.
static const int kInstErrorBindlessBounds = 0;
static const int kInstErrorBindlessUninit = 1;
static const int kInstErrorBuffAddrUnallocRef = 2;

// Direct Input Buffer Offsets
//
// The following values provide member offsets into the input buffers
// consumed by InstrumentPass::GenDebugDirectRead(). This method is utilized
// by InstBindlessCheckPass.
//
// The only object in an input buffer is a runtime array of unsigned
// integers. Each validation will have its own formatting of this array.
static const int kDebugInputDataOffset = 0;

// Debug Buffer Bindings
//
// These are the bindings for the different buffers which are
// read or written by the instrumentation passes.
//
// This is the output buffer written by InstBindlessCheckPass,
// InstBuffAddrCheckPass, and possibly other future validations.
static const int kDebugOutputBindingStream = 0;

// The binding for the input buffer read by InstBindlessCheckPass.
static const int kDebugInputBindingBindless = 1;

// The binding for the input buffer read by InstBuffAddrCheckPass.
static const int kDebugInputBindingBuffAddr = 2;

// This is the output buffer written by InstDebugPrintfPass.
static const int kDebugOutputPrintfStream = 3;

// Bindless Validation Input Buffer Format
//
// An input buffer for bindless validation consists of a single array of
// unsigned integers we will call Data[]. This array is formatted as follows.
//
// At offset kDebugInputBindlessInitOffset in Data[] is a single uint which
// gives an offset to the start of the bindless initialization data. More
// specifically, if the following value is zero, we know that the descriptor at
// (set = s, binding = b, index = i) is not initialized:
// Data[ i + Data[ b + Data[ s + Data[ kDebugInputBindlessInitOffset ] ] ] ]
static const int kDebugInputBindlessInitOffset = 0;

// At offset kDebugInputBindlessOffsetLengths is some number of uints which
// provide the bindless length data. More specifically, the number of
// descriptors at (set=s, binding=b) is:
// Data[ Data[ s + kDebugInputBindlessOffsetLengths ] + b ]
static const int kDebugInputBindlessOffsetLengths = 1;

// Buffer Device Address Input Buffer Format
//
// An input buffer for buffer device address validation consists of a single
// array of unsigned 64-bit integers we will call Data[]. This array is
// formatted as follows:
//
// At offset kDebugInputBuffAddrPtrOffset is a list of sorted valid buffer
// addresses. The list is terminated with the address 0xffffffffffffffff.
// If 0x0 is not a valid buffer address, this address is inserted at the
// start of the list.
//
static const int kDebugInputBuffAddrPtrOffset = 1;
//
// At offset kDebugInputBuffAddrLengthOffset in Data[] is a single uint64 which
// gives an offset to the start of the buffer length data. More
// specifically, for a buffer whose pointer is located at input buffer offset
// i, the length is located at:
//
// Data[ i - kDebugInputBuffAddrPtrOffset
//         + Data[ kDebugInputBuffAddrLengthOffset ] ]
//
// The length associated with the 0xffffffffffffffff address is zero. If
// not a valid buffer, the length associated with the 0x0 address is zero.
static const int kDebugInputBuffAddrLengthOffset = 0;

}  // namespace spvtools

#endif  // INCLUDE_SPIRV_TOOLS_INSTRUMENT_HPP_
