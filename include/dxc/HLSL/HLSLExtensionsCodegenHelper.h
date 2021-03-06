///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLSLExtensionsCodegenHelper.h                                             //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// Licensed under the MIT license. See COPYRIGHT in the project root for     //
// full license information.                                                 //
//                                                                           //
// Codegen support for hlsl extensions.                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>
#include <string>

namespace llvm {
class CallInst;
class Value;
class Module;
}

namespace hlsl {

// Provide DXIL codegen support for private HLSL extensions.
// The HLSL extension mechanism has hooks for two cases:
//
//  1. You can mark certain defines as "semantic" defines which
//     will be preserved as metadata in the final DXIL.
//  2. You can add new HLSL intrinsic functions.
//
// This class provides an interface for generating the DXIL bitcode
// needed for the two types of extensions above.
//  
class HLSLExtensionsCodegenHelper {
public:
  // Used to indicate a semantic define was used incorrectly.
  // Since semantic defines have semantic meaning it is possible
  // that a programmer can use them incorrectly. This class provides
  // a way to signal the error to the programmer. Semantic define
  // errors will be propagated as errors to the clang frontend.
  class SemanticDefineError {
  public:
    enum class Level { Warning, Error };
    SemanticDefineError(unsigned location, Level level, const std::string &message)
    :  m_location(location)
    ,  m_level(level)
    ,  m_message(message)
    { }

    unsigned Location() const { return m_location; }
    bool IsWarning() const { return m_level == Level::Warning; }
    const std::string &Message() const { return m_message; }

  private:
    unsigned m_location; // Use an encoded clang::SourceLocation to avoid a clang include dependency.
    Level m_level;
    std::string m_message;
  };
  typedef std::vector<SemanticDefineError> SemanticDefineErrorList;

  // Write semantic defines as metadata in the module.
  virtual SemanticDefineErrorList WriteSemanticDefines(llvm::Module *M) = 0;

  // Get the name to use for the dxil intrinsic function.
  virtual std::string GetIntrinsicName(unsigned opcode) = 0;

  // Virtual destructor.
  virtual ~HLSLExtensionsCodegenHelper() {};
};
}
