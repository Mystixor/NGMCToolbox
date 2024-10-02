#pragma once

namespace NGMC
{
    struct Vec2;
    float operator*(const Vec2& left, const Vec2& right);
    struct Vec2 {
        float x, y;

        Vec2 operator-() const
        {
            return { -x, -y };
        }

        float Length() const
        {
            return std::sqrtf((*this) * (*this));
        }
    };

    float operator*(const Vec2& left, const Vec2& right)
    {
        return left.x * right.x + left.y * right.y;
    }
    Vec2 operator*(const Vec2& left, float right)
    {
        return { left.x * right, left.y * right };
    }
    Vec2 operator*(float left, const Vec2& right)
    {
        return { left * right.x, left * right.y };
    }
    Vec2 operator/(const Vec2& left, float right)
    {
        return { left.x / right, left.y / right };
    }
    Vec2 operator+(const Vec2& left, const Vec2& right)
    {
        return { left.x + right.x, left.y + right.y };
    }
    Vec2 operator-(const Vec2& left, const Vec2& right)
    {
        return { left.x - right.x, left.y - right.y };
    }

    struct Vec3;
    float operator*(const Vec3& left, const Vec3& right);
    struct Vec3 {
        float x, y, z;

        Vec3 operator-() const
        {
            return { -x, -y, -z };
        }

        float Length() const
        {
            return std::sqrtf(*this * *this);
        }
    };

    float operator*(const Vec3& left, const Vec3& right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z;
    }
    Vec3 operator*(const Vec3& left, float right)
    {
        return { left.x * right, left.y * right, left.z * right };
    }
    Vec3 operator*(float left, const Vec3& right)
    {
        return { left * right.x, left * right.y, left * right.z };
    }
    Vec3 operator/(const Vec3& left, float right)
    {
        return { left.x / right, left.y / right, left.z / right };
    }
    Vec3 operator+(const Vec3& left, const Vec3& right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z };
    }
    Vec3 operator-(const Vec3& left, const Vec3& right)
    {
        return { left.x - right.x, left.y - right.y, left.z - right.z };
    }
    Vec3 Cross(const Vec3& left, const Vec3& right)
    {
        return {
            left.y * right.z - left.z * right.y,
            left.z * right.x - left.x * right.z,
            left.x * right.y - left.y * right.x
        };
    }


    struct Vec4;
    float operator*(const Vec4& left, const Vec4& right);
    struct Vec4 {
        float x, y, z, w;

        Vec4 operator-() const
        {
            return { -x, -y, -z, -w };
        }

        float Length() const
        {
            return std::sqrtf(*this * *this);
        }
    };

    float operator*(const Vec4& left, const Vec4& right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
    }
    Vec4 operator*(const Vec4& left, float right)
    {
        return { left.x * right, left.y * right, left.z * right, left.w * right };
    }
    Vec4 operator*(float left, const Vec4& right)
    {
        return { left * right.x, left * right.y, left * right.z, left * right.w };
    }
    Vec4 operator/(const Vec4& left, float right)
    {
        return { left.x / right, left.y / right, left.z / right, left.w / right };
    }
    Vec4 operator+(const Vec4& left, const Vec4& right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
    }
    Vec4 operator-(const Vec4& left, const Vec4& right)
    {
        return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
    }


    struct Mat2x2 {
        Vec2 x, y;

        Mat2x2 operator-() const
        {
            return { -x, -y };
        }

        Mat2x2 Transpose() const
        {
            return {
                { x.x, y.x },
                { x.y, y.y }
            };
        }
    };

    Mat2x2 operator*(const Mat2x2& left, const Mat2x2& right)
    {
        Mat2x2 t = left.Transpose();
        return {
            {t.x * right.x, t.y * right.x },
            {t.x * right.y, t.y * right.y }
        };
    }
    Vec2 operator*(const Mat2x2& left, const Vec2& right)
    {
        Mat2x2 t = left.Transpose();
        return { t.x * right, t.y * right };
    }
    Mat2x2 operator*(const Mat2x2& left, float right)
    {
        return { left.x * right, left.y * right };
    }
    Mat2x2 operator*(float left, const Mat2x2& right)
    {
        return { left * right.x, left * right.y };
    }
    Mat2x2 operator/(const Mat2x2& left, float right)
    {
        return { left.x / right, left.y / right };
    }
    Mat2x2 operator+(const Mat2x2& left, const Mat2x2& right)
    {
        return { left.x + right.x, left.y + right.y };
    }
    Mat2x2 operator-(const Mat2x2& left, const Mat2x2& right)
    {
        return { left.x - right.x, left.y - right.y };
    }
    

    struct Mat3x3 {
        Vec3 x, y, z;

        Mat3x3 operator-() const
        {
            return { -x, -y, -z };
        }

        Mat3x3 Transpose() const
        {
            return {
                { x.x, y.x, z.x },
                { x.y, y.y, z.y },
                { x.z, y.z, z.z }
            };
        }
    };

    Mat3x3 operator*(const Mat3x3& left, const Mat3x3& right)
    {
        Mat3x3 t = left.Transpose();
        return {
            {t.x * right.x, t.y * right.x, t.z * right.x },
            {t.x * right.y, t.y * right.y, t.z * right.y },
            {t.x * right.z, t.y * right.z, t.z * right.z }
        };
    }
    Vec3 operator*(const Mat3x3& left, const Vec3& right)
    {
        Mat3x3 t = left.Transpose();
        return { t.x * right, t.y * right, t.z * right };
    }
    Mat3x3 operator*(const Mat3x3& left, float right)
    {
        return { left.x * right, left.y * right, left.z * right };
    }
    Mat3x3 operator*(float left, const Mat3x3& right)
    {
        return { left * right.x, left * right.y, left * right.z };
    }
    Mat3x3 operator/(const Mat3x3& left, float right)
    {
        return { left.x / right, left.y / right, left.z / right };
    }
    Mat3x3 operator+(const Mat3x3& left, const Mat3x3& right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z };
    }
    Mat3x3 operator-(const Mat3x3& left, const Mat3x3& right)
    {
        return { left.x - right.x, left.y - right.y, left.z - right.z };
    }


    struct Mat4x4 {
        Vec4 x, y, z, w;

        Mat4x4 operator-() const
        {
            return { -x, -y, -z, -w };
        }

        Mat4x4 Transpose() const
        {
            return {
                { x.x, y.x, z.x, w.x },
                { x.y, y.y, z.y, w.y },
                { x.z, y.z, z.z, w.z },
                { x.w, y.w, z.w, w.w }
            };
        }
    };

    Mat4x4 operator*(const Mat4x4& left, const Mat4x4& right)
    {
        Mat4x4 t = left.Transpose();
        return {
            {t.x * right.x, t.y * right.x, t.z * right.x, t.w * right.x },
            {t.x * right.y, t.y * right.y, t.z * right.y, t.w * right.y },
            {t.x * right.z, t.y * right.z, t.z * right.z, t.w * right.z },
            {t.x * right.w, t.y * right.w, t.z * right.w, t.w * right.w }
        };
    }
    Vec4 operator*(const Mat4x4& left, const Vec4& right)
    {
        Mat4x4 t = left.Transpose();
        return { t.x * right, t.y * right, t.z * right, t.w * right };
    }
    Mat4x4 operator*(const Mat4x4& left, float right)
    {
        return { left.x * right, left.y * right, left.z * right, left.w * right };
    }
    Mat4x4 operator*(float left, const Mat4x4& right)
    {
        return { left * right.x, left * right.y, left * right.z, left * right.w };
    }
    Mat4x4 operator/(const Mat4x4& left, float right)
    {
        return { left.x / right, left.y / right, left.z / right, left.w / right };
    }
    Mat4x4 operator+(const Mat4x4& left, const Mat4x4& right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
    }
    Mat4x4 operator-(const Mat4x4& left, const Mat4x4& right)
    {
        return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
    }
}