#include <glad/glad.h>

#include "../loading/objLoader.h"
#include "entity.h"
#include "../models/texturedmodel.h"
#include "stagecollision.h"
#include "../loading/loader.h"
#include "../main/main.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"
#include "../collision/triangle3d.h"

#include <list>
#include <format>

std::list<TexturedModel*> StageCollision::models;
CollisionModel* StageCollision::collisionModel;

StageCollision::StageCollision()
{
    position.x = 0;
    position.y = 0;
    position.z = 0;
    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
    visible = true;
    baseColour.set(0.70f, 0.70f, 0.70f);
    updateTransformationMatrixYXZ();

}


void StageCollision::step()
{
    visible = Global::displayStageCollision;
    if (StageCollision::collisionModel != nullptr)
    {
        StageCollision::collisionModel->isVisible = visible;
    }
}

std::list<TexturedModel*>* StageCollision::getModels()
{
    return &StageCollision::models;
}

void StageCollision::loadModels(const char* objFolder, const char* objFilename)
{
    if (StageCollision::models.size() == 0)
    {
        #ifdef DEV_MODE
        std::fprintf(stdout, "Loading StageCollision static models...\n");
        #endif

        loadModel(&StageCollision::models, objFolder, objFilename);
    }

    StageCollision::collisionModel = loadCollisionModel(objFolder, objFilename);
    StageCollision::collisionModel->parent = Global::gameStageCollision;
    CollisionChecker::addCollideModel(StageCollision::collisionModel);
}

void StageCollision::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting StageCollision static models...\n");
    #endif

    Entity::deleteModels(&StageCollision::models);
}

// yoinked from dolphin MathUtil.cpp
const int frsqrte_expected_base[] =
{
    0x3ffa000, 0x3c29000, 0x38aa000, 0x3572000,
    0x3279000, 0x2fb7000, 0x2d26000, 0x2ac0000,
    0x2881000, 0x2665000, 0x2468000, 0x2287000,
    0x20c1000, 0x1f12000, 0x1d79000, 0x1bf4000,
    0x1a7e800, 0x17cb800, 0x1552800, 0x130c000,
    0x10f2000, 0x0eff000, 0x0d2e000, 0x0b7c000,
    0x09e5000, 0x0867000, 0x06ff000, 0x05ab800,
    0x046a000, 0x0339800, 0x0218800, 0x0105800,
};
const int frsqrte_expected_dec[] =
{
    0x7a4, 0x700, 0x670, 0x5f2,
    0x584, 0x524, 0x4cc, 0x47e,
    0x43a, 0x3fa, 0x3c2, 0x38e,
    0x35e, 0x332, 0x30a, 0x2e6,
    0x568, 0x4f3, 0x48d, 0x435,
    0x3e7, 0x3a2, 0x365, 0x32e,
    0x2fc, 0x2d0, 0x2a8, 0x283,
    0x261, 0x243, 0x226, 0x20b,
};

double gcApproxInvSqrt(double val) {
    union
    {
        double valf;
        int64_t vali;
    };
    valf = val;
    int64_t mantissa = vali & ((1LL << 52) - 1);
    int64_t sign = vali & (1ULL << 63);
    int64_t exponent = vali & (0x7FFLL << 52);

    // Special case 0
    if (mantissa == 0 && exponent == 0)
        return sign ? -std::numeric_limits<double>::infinity() :
        std::numeric_limits<double>::infinity();
    // Special case NaN-ish numbers
    if (exponent == (0x7FFLL << 52))
    {
        if (mantissa == 0)
        {
            if (sign)
                return std::numeric_limits<double>::quiet_NaN();

            return 0.0;
        }

        return 0.0 + valf;
    }

    // Negative numbers return NaN
    if (sign)
        return std::numeric_limits<double>::quiet_NaN();

    if (!exponent)
    {
        // "Normalize" denormal values
        do
        {
            exponent -= 1LL << 52;
            mantissa <<= 1;
        } while (!(mantissa & (1LL << 52)));
        mantissa &= (1LL << 52) - 1;
        exponent += 1LL << 52;
    }

    bool odd_exponent = !(exponent & (1LL << 52));
    exponent = ((0x3FFLL << 52) - ((exponent - (0x3FELL << 52)) / 2)) & (0x7FFLL << 52);

    int i = (int)(mantissa >> 37);
    vali = sign | exponent;
    int index = i / 2048 + (odd_exponent ? 16 : 0);
    vali |= (int64_t)(frsqrte_expected_base[index] - frsqrte_expected_dec[index] * (i % 2048)) << 26;
    return valf;
}

const int fres_expected_base[] =
{
    0x7ff800, 0x783800, 0x70ea00, 0x6a0800,
    0x638800, 0x5d6200, 0x579000, 0x520800,
    0x4cc800, 0x47ca00, 0x430800, 0x3e8000,
    0x3a2c00, 0x360800, 0x321400, 0x2e4a00,
    0x2aa800, 0x272c00, 0x23d600, 0x209e00,
    0x1d8800, 0x1a9000, 0x17ae00, 0x14f800,
    0x124400, 0x0fbe00, 0x0d3800, 0x0ade00,
    0x088400, 0x065000, 0x041c00, 0x020c00,
};
const int fres_expected_dec[] =
{
    0x3e1, 0x3a7, 0x371, 0x340,
    0x313, 0x2ea, 0x2c4, 0x2a0,
    0x27f, 0x261, 0x245, 0x22a,
    0x212, 0x1fb, 0x1e5, 0x1d1,
    0x1be, 0x1ac, 0x19b, 0x18b,
    0x17c, 0x16e, 0x15b, 0x15b,
    0x143, 0x143, 0x12d, 0x12d,
    0x11a, 0x11a, 0x108, 0x106,
};

float gcApproxInv(float val) {
    union
    {
        double valf;
        int64_t vali;
    };

    valf = val;
    int64_t mantissa = vali & ((1LL << 52) - 1);
    int64_t sign = vali & (1ULL << 63);
    int64_t exponent = vali & (0x7FFLL << 52);

    // Special case 0
    if (mantissa == 0 && exponent == 0)
        return sign ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();

    // Special case NaN-ish numbers
    if (exponent == (0x7FFLL << 52))
    {
        if (mantissa == 0)
            return sign ? -0.0 : 0.0;
        return 0.0 + valf;
    }

    // Special case small inputs
    if (exponent < (895LL << 52))
        return sign ? -std::numeric_limits<float>::max() : std::numeric_limits<float>::max();

    // Special case large inputs
    if (exponent >= (1149LL << 52))
        return sign ? -0.0f : 0.0f;

    exponent = (0x7FDLL << 52) - exponent;

    int i = (int)(mantissa >> 37);
    vali = sign | exponent;
    vali |= (int64_t)(fres_expected_base[i / 1024] - (fres_expected_dec[i / 1024] * (i % 1024) + 1) / 2) << 29;
    return valf;
}

Vector3f generateGamecubeNormals(Triangle3D* tri) {
    Vector3f retVec;

    // Matching what njUnitVector does
    float sum = tri->A * tri->A;
    sum = std::fmaf(tri->B, tri->B, sum);
    sum = std::fmaf(tri->C, tri->C, sum);

    float invSqrt = gcApproxInvSqrt(sum);

    float mag = gcApproxInv(invSqrt);

    if (mag != 0) {
        retVec.x = tri->A * invSqrt;
        retVec.y = tri->B * invSqrt;
        retVec.z = tri->C * invSqrt;
    }
    else {
        retVec.x = 0;
        retVec.y = 1;
        retVec.z = 0;
    }
    return retVec;
}

void StageCollision::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[0], "Triangle Collision Info");
    SetWindowTextA(Global::windowLabels[1], "");
    SetWindowTextA(Global::windowLabels[2], "PC Norm.X");
    SetWindowTextA(Global::windowLabels[3], "PC Norm.Y");
    SetWindowTextA(Global::windowLabels[4], "PC Norm.Z");
    SetWindowTextA(Global::windowLabels[5], "GC Norm.X");
    SetWindowTextA(Global::windowLabels[6], "GC Norm.Y");
    SetWindowTextA(Global::windowLabels[7], "GC Norm.Z");
    SetWindowTextA(Global::windowLabels[8], "");
    SetWindowTextA(Global::windowLabels[9], "");
    SetWindowTextA(Global::windowLabels[10], "");

    SetWindowTextA(Global::windowValues[0], "");
    SetWindowTextA(Global::windowValues[1], "");

    Triangle3D *collided = CollisionChecker::getCollideTriangle();
    SetWindowTextA(Global::windowValues[2], std::format("{:.12f}", collided->normal.getX()).c_str());
    SetWindowTextA(Global::windowValues[3], std::format("{:.12f}", collided->normal.getY()).c_str());
    SetWindowTextA(Global::windowValues[4], std::format("{:.12f}", collided->normal.getZ()).c_str());

    Vector3f gcNorm = generateGamecubeNormals(collided);
    SetWindowTextA(Global::windowValues[5], std::format("{:.12f}", gcNorm.getX()).c_str());
    SetWindowTextA(Global::windowValues[6], std::format("{:.12f}", gcNorm.getY()).c_str());
    SetWindowTextA(Global::windowValues[7], std::format("{:.12f}", gcNorm.getZ()).c_str());

    SetWindowTextA(Global::windowValues[8], "");
    SetWindowTextA(Global::windowValues[9], "");
    SetWindowTextA(Global::windowValues[10], "");

    SendMessageA(Global::windowValues[0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[5], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[6], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[7], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[8], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[9], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[0], "");
    SetWindowTextA(Global::windowDescriptions[1], "");
    SetWindowTextA(Global::windowDescriptions[2], "");
    SetWindowTextA(Global::windowDescriptions[3], "");
    SetWindowTextA(Global::windowDescriptions[4], "");
    SetWindowTextA(Global::windowDescriptions[5], "");
    SetWindowTextA(Global::windowDescriptions[6], "");
    SetWindowTextA(Global::windowDescriptions[7], "");
    SetWindowTextA(Global::windowDescriptions[8], "");
    SetWindowTextA(Global::windowDescriptions[9], "");
    SetWindowTextA(Global::windowDescriptions[10], "");

    updateTransformationMatrixYXZ();
}
