// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenColorIO Project.

#include <OpenColorIO/OpenColorIO.h>

#include "GPUUnitTest.h"

namespace OCIO = OCIO_NAMESPACE;


const int LUT3D_EDGE_SIZE = 32;
const float g_epsilon = 1e-6f;


enum Version : unsigned
{
    OCIO_VERSION_1 = 1,
    OCIO_VERSION_2 = 2  // version 2 or higher
};

// Helper method to build unit tests
void AddExponent(OCIOGPUTest & test,
                 OCIO::GpuShaderDescRcPtr & shaderDesc,
                 OCIO::TransformDirection direction,
                 const double(&gamma)[4],
                 OCIO::NegativeStyle style,
                 float epsilon,
                 Version ver)
{
    OCIO::ExponentTransformRcPtr exp = OCIO::ExponentTransform::Create();
    exp->setNegativeStyle(style);
    exp->setDirection(direction);
    exp->setValue(gamma);

    OCIO_NAMESPACE::ConfigRcPtr config = OCIO_NAMESPACE::Config::Create();
    config->setMajorVersion(ver);

    test.setErrorThreshold(epsilon);
    test.setContext(config, exp->createEditableCopy(), shaderDesc);
}

// Helper method to build unit tests
void AddExponentWithLinear(OCIOGPUTest & test,
                           OCIO::GpuShaderDescRcPtr & shaderDesc,
                           OCIO::TransformDirection direction,
                           const double(&gamma)[4],
                           const double(&offset)[4],
                           OCIO::NegativeStyle style,
                           float epsilon)
{
    OCIO::ExponentWithLinearTransformRcPtr exp = OCIO::ExponentWithLinearTransform::Create();
    exp->setDirection(direction);
    exp->setGamma(gamma);
    exp->setOffset(offset);
    exp->setNegativeStyle(style);

    OCIO_NAMESPACE::ConfigRcPtr config = OCIO_NAMESPACE::Config::Create();
    config->setMajorVersion(2);

    test.setErrorThreshold(epsilon);
    test.setContext(config, exp->createEditableCopy(), shaderDesc);
}


OCIO_ADD_GPU_TEST(ExponentOp, legacy_shader_v1)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc
        = OCIO::GpuShaderDesc::CreateLegacyShaderDesc(LUT3D_EDGE_SIZE);

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, exp, OCIO::NEGATIVE_CLAMP, 1e-5f,
                OCIO_VERSION_1);
    // TODO: Would like to be able to remove the setTestNaN(false) and
    // setTestInfinity(false) from all of these tests.
    test.setTestNaN(false);
}

OCIO_ADD_GPU_TEST(ExponentOp, forward_v1)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, exp, OCIO::NEGATIVE_CLAMP, 1e-5f,
                OCIO_VERSION_1);
    test.setTestNaN(false);
}

OCIO_ADD_GPU_TEST(ExponentOp, forward)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, exp, OCIO::NEGATIVE_CLAMP,
#ifdef USE_SSE
        5e-4f
#else
        1e-5f
#endif
        , OCIO_VERSION_2);
}

OCIO_ADD_GPU_TEST(ExponentOp, forward_mirror)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, exp, OCIO::NEGATIVE_MIRROR,
#ifdef USE_SSE
        5e-4f // TODO: Only related to the ssePower optimization ?
#else
        1e-5f
#endif
        , OCIO_VERSION_2);
}

OCIO_ADD_GPU_TEST(ExponentOp, forward_pass_thru)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, exp, OCIO::NEGATIVE_PASS_THRU,
#ifdef USE_SSE
        5e-4f // TODO: Only related to the ssePower optimization ?
#else
        1e-5f
#endif
        , OCIO_VERSION_2);
}

OCIO_ADD_GPU_TEST(ExponentOp, inverse_legacy_shader_v1)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc
        = OCIO::GpuShaderDesc::CreateLegacyShaderDesc(LUT3D_EDGE_SIZE);

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, exp, OCIO::NEGATIVE_CLAMP,
                g_epsilon, OCIO_VERSION_1);
    test.setTestNaN(false);
}

OCIO_ADD_GPU_TEST(ExponentOp, inverse_v1)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, exp, OCIO::NEGATIVE_CLAMP,
                g_epsilon, OCIO_VERSION_1);
    test.setTestNaN(false);
}

OCIO_ADD_GPU_TEST(ExponentOp, inverse)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, exp, OCIO::NEGATIVE_CLAMP,
#ifdef USE_SSE
        5e-4f // TODO: Only related to the ssePower optimization ?
#else
        g_epsilon
#endif
        , OCIO_VERSION_2);
    test.setTestInfinity(false);
}

OCIO_ADD_GPU_TEST(ExponentOp, inverse_mirror)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, exp, OCIO::NEGATIVE_MIRROR,
#ifdef USE_SSE
        5e-4f // TODO: Only related to the ssePower optimization ?
#else
        g_epsilon
#endif
        , OCIO_VERSION_2);
    test.setTestInfinity(false);
}

OCIO_ADD_GPU_TEST(ExponentOp, inverse_pass_thru)
{
    const double exp[4] = { 2.6, 1.0, 1.8, 1.1 };

    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponent(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, exp, OCIO::NEGATIVE_PASS_THRU,
#ifdef USE_SSE
        5e-4f // TODO: Only related to the ssePower optimization ?
#else
        g_epsilon
#endif
        , OCIO_VERSION_2);
    test.setTestInfinity(false);
}

const double gamma[4]  = { 2.1,  1.0,  2.3,  1.5  };
const double offset[4] = {  .01, 0.,    .03,  .05 };

OCIO_ADD_GPU_TEST(ExponentWithLinearOp, forward)
{
    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponentWithLinear(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, gamma, offset,
        OCIO::NEGATIVE_LINEAR,
#ifdef USE_SSE
        1e-4f // Note: Related to the ssePower optimization !
#else
        5e-6f
#endif
        );
    test.setTestInfinity(false);
}

OCIO_ADD_GPU_TEST(ExponentWithLinearOp, mirror_forward)
{
    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponentWithLinear(test, shaderDesc, OCIO::TRANSFORM_DIR_FORWARD, gamma, offset,
        OCIO::NEGATIVE_MIRROR,
#ifdef USE_SSE
        1e-4f // Note: Related to the ssePower optimization !
#else
        5e-6f
#endif
    );
    test.setTestInfinity(false);
}

OCIO_ADD_GPU_TEST(ExponentWithLinearOp, inverse)
{
    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponentWithLinear(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, gamma, offset,
        OCIO::NEGATIVE_LINEAR,
#ifdef USE_SSE
        5e-5f // Note: Related to the ssePower optimization !
#else
        5e-7f
#endif
        );
    test.setTestInfinity(false);
}

OCIO_ADD_GPU_TEST(ExponentWithLinearOp, mirror_inverse)
{
    OCIO::GpuShaderDescRcPtr shaderDesc = OCIO::GpuShaderDesc::CreateShaderDesc();

    AddExponentWithLinear(test, shaderDesc, OCIO::TRANSFORM_DIR_INVERSE, gamma, offset,
        OCIO::NEGATIVE_MIRROR,
#ifdef USE_SSE
        5e-5f // Note: Related to the ssePower optimization !
#else
        5e-7f
#endif
    );
    test.setTestInfinity(false);
}

