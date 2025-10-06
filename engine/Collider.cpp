#include "Collider.h"
#include"Camera.h"
#include"Controller.h"
#include<easyx.h>
#include"ColliderManager.h"
#include"Math.h"
#include"RigidBody.h"


bool (*Collider::collisionJudgeMap[3])(Collider*, Collider*) =
{ &Collider::collisionJudgeCircleToCircle,Collider::collisionJudgeCircleToBox,Collider::collisionJudgeBoxToBox };

HitResult(*Collider::collisionHitMap[3])(Collider*, Collider*) =
{ &Collider::collisionHitCircleToCircle,Collider::collisionHitCircleToBox,Collider::collisionHitBoxToBox };


void CircleCollider::Update()
{
    Collider::Update();

    radius = radius_ini * sqrtf(GetWorldScale().x * GetWorldScale().y);
}

void CircleCollider::DrawDebugLine()
{
    Vector2D pos = (GetWorldPosition() - mainWorld.mainCamera->GetCameraPosition()) * 20.f / mainWorld.mainCamera->springArmLength_virtual + Vector2D(WIN_WIDTH / 2, WIN_HEIGHT / 2);
    circle((int)pos.x, (int)pos.y, int(radius * 20.f / mainWorld.mainCamera->springArmLength));
}

bool CircleCollider::IsMouseOver()
{
    return Vector2D::Distance(GetWorldPosition(),mainWorld.mainController->GetCursorPosition()) <= radius;
}

void CircleCollider::ReSetRect()
{
    Rcenter = GetWorldPosition();
	Rwidth = Rheight = radius * 2;
}






void BoxCollider::Update()
{
    Collider::Update();

    size = size_ini ;
    Vector2D pos = GetWorldPosition();
    rect = { pos.x - size.x / 2 ,pos.y + size.y / 2 ,pos.x + size.x / 2 ,pos.y - size.y / 2 };
}


void BoxCollider::DrawDebugLine()
{
    Vector2D pos = (GetWorldPosition() - mainWorld.mainCamera->GetCameraPosition())
        * 20.f / mainWorld.mainCamera->springArmLength_virtual
        + Vector2D(WIN_WIDTH / 2, WIN_HEIGHT / 2);

    Vector2D si = this->size * 20.f / mainWorld.mainCamera->springArmLength_virtual;
    Vector2D halfSize = si / 2.0f;

    Vector2D corners[4] = {
        {-halfSize.x, -halfSize.y},
        { halfSize.x, -halfSize.y},
        { halfSize.x,  halfSize.y},
        {-halfSize.x,  halfSize.y},
    };

    float radian = PI * GetWorldRotation() / 180.0f;
    float cs = cos(radian), sn = sin(radian);

    for (int i = 0; i < 4; i++)
    {
        float x = corners[i].x;
        float y = corners[i].y;
        corners[i].x = x * cs - y * sn + pos.x;
        corners[i].y = x * sn + y * cs + pos.y;
    }

    for (int i = 0; i < 4; i++)
        line((int)corners[i].x, (int)corners[i].y,
            (int)corners[(i + 1) % 4].x, (int)corners[(i + 1) % 4].y);
}

bool BoxCollider::IsMouseOver()
{
    Vector2D pos = GetWorldPosition(), pos_m = mainWorld.mainController->GetCursorPosition();
    return pos_m.x <= pos.x + size.x / 2 && pos_m.x >= pos.x - size.x / 2 &&
        pos_m.y <= pos.y + size.y / 2 && pos_m.y >= pos.y - size.y / 2;
}

Vector2D BoxCollider::GetOverlapRect(const Rect& r1, const Rect& r2)
{
    return { min(r1.right, r2.right) - max(r1.left, r2.left), min(r1.bottom, r2.bottom)-max(r1.top, r2.top) };
}





void BoxCollider::ReSetRect()
{
	float r = fmod(GetWorldRotation(), 360.f);
	if (r < 0)r += 360.f;
    double radian = PI * r / 180;
    float fsin = float(sin(radian));
    float fcos = float(cos(radian));
    if (r == 0) {
        Rwidth = size.x; 
        Rheight = size.y;
		Rcenter = GetWorldPosition();
    }
    else {
        Rwidth = fabs(size.x * fcos) + fabs(size.y * fsin);
        Rheight = fabs(size.x * fsin) + fabs(size.y * fcos);
        Rcenter = GetWorldPosition();
    }
}

std::vector<Vector2D> BoxCollider::GetWorldVertices() const
{
    Vector2D center = GetWorldPosition();
    Vector2D half = GetSize() / 2.0f;

    std::vector<Vector2D> localVerts = {
        {-half.x, -half.y}, { half.x, -half.y},
        { half.x,  half.y}, {-half.x,  half.y}
    };

    float angle = GetWorldRotation();
    std::vector<Vector2D> worldVerts;
    worldVerts.reserve(4);

    for (auto& v : localVerts) {
        Vector2D rotated = Vector2D::RotateVector(angle, v);
        worldVerts.push_back(center + rotated);
    }
    return worldVerts;
}

void BoxCollider::ProjectPolygon(const Vector2D& axis, const std::vector<Vector2D>& verts, float& min, float& max)
{
    float dot = Vector2D::DotProduct(verts[0], axis);
    min = max = dot;
    for (size_t i = 1; i < verts.size(); i++) {
        dot = Vector2D::DotProduct(verts[i], axis);
        if (dot < min) min = dot;
        if (dot > max) max = dot;
    }
}

void Collider::Update()
{
    SceneComponent::Update();

    if (mode == CollisionMode::None)return;
    
}

void Collider::BeginPlay()
{
    
    rigidAttached = pOwner->GetComponentByClass<RigidBody>();
    
    
    if (rigidAttached) 
    {
        rigidAttached->colliders.insert(this);
        
    }
}

void Collider::Clear()
{
    for (auto& another : collisions)
    {
        another->collisions.erase(this);
        if (another->mode == CollisionMode::Collision && this->mode == CollisionMode::Collision)continue;
        OnComponentEndOverlap.BroadCast(this, another, another->pOwner);  another->OnComponentEndOverlap.BroadCast(another, this, pOwner);
    }
    collisions.clear();
    if (point != Pair(-1, -1))
    {
        for (int i = point.x; i <= point_1.x; ++i)for (int j = point.y; j <= point_1.y; ++j)mainWorld.ColliderZones[i][j].erase(this);
    }
    point = { -1, -1 }, point_1 = { -1, -1 };
}

void Collider::Erase()
{
    collisions_to_erase.clear();
    for (auto& another : collisions) {
        if (!CollisionJudge(another)) {
            another->collisions.erase(this); collisions_to_erase.push_back(another);
            if (another->mode == CollisionMode::Collision && this->mode == CollisionMode::Collision)continue;
            OnComponentEndOverlap.BroadCast(this, another, another->pOwner); another->OnComponentEndOverlap.BroadCast(another, this, pOwner);
        }
    }
    for (auto& another : collisions_to_erase) collisions.erase(another);
}

void Collider::Insert(Collider* another)
{
    
    if (mainWorld.collisionManager->FindMapping(this->type, another->type)
        && collisions.find(another) == collisions.end() && CollisionJudge(another))
    {
        collisions.insert(another); another->collisions.insert(this);
        if (another->mode == CollisionMode::Collision && this->mode == CollisionMode::Collision)
        {
            HitResult hitResult = this->CollisionHit(another);
            /*if (rigidAttached)rigidAttached->RestrictVelocity(-hitResult.ImpactNormal,another->rigidAttached);*/
            OnComponentHit.BroadCast(this, another, another->pOwner, -hitResult.ImpactNormal, hitResult);
            another->OnComponentHit.BroadCast(another, this, pOwner, hitResult.ImpactNormal, { hitResult.ImpactPoint,-hitResult.ImpactNormal,pOwner,this,0 });
        }
        else
        {
            OnComponentBeginOverlap.BroadCast(this, another, another->pOwner); another->OnComponentBeginOverlap.BroadCast(another, this, pOwner);
        }
    }
}

HitResult Collider::CollisionHit(Collider* another)
{
    int shape1 = int(this->shape), shape2 = int(another->shape);
    return collisionHitMap[shape1 * shape1 + shape2 * shape2](this, another);
}

HitResult Collider::collisionHitCircleToCircle(Collider* c1, Collider* c2)
{
    CircleCollider* circle1 = Cast<CircleCollider>(c1);
    CircleCollider* circle2 = Cast<CircleCollider>(c2);
    Vector2D d = circle2->GetWorldPosition() - circle1->GetWorldPosition();
    float dist = d.Size();
    Vector2D impactNormal = d.Normalize();

    float penetration = circle1->GetRadius() + circle2->GetRadius() - dist;

    Vector2D impactPoint = circle1->GetWorldPosition() + impactNormal * circle1->GetRadius();

    return HitResult(impactPoint, impactNormal, circle2->pOwner, circle2, penetration);
}

HitResult Collider::collisionHitCircleToBox(Collider* c1, Collider* c2)
{

    CircleCollider* circle; BoxCollider* box;
    if (c1->GetShape() == Circle)
    {
        circle = Cast<CircleCollider>(c1), box = Cast<BoxCollider>(c2);
    }
    else
    {
        circle = Cast<CircleCollider>(c2), box = Cast<BoxCollider>(c1);
    }

    float radius = circle->GetRadius();
    Vector2D pos = circle->GetWorldPosition();
    BoxCollider::Rect rect = box->GetRect();
    Vector2D boxPos = box->GetWorldPosition();
    float angle = box->GetWorldRotation();

    // 将圆心坐标旋转到矩形局部空间
    Vector2D d = pos - boxPos;
    pos = boxPos + Vector2D::RotateVector(-angle, d);

    Vector2D impactNormal;
    Vector2D impactPoint;
    float penetration;

    // 圆心在矩形内部
    if (pos.x <= rect.right && pos.x >= rect.left && pos.y <= rect.top && pos.y >= rect.bottom)
    {
        float dxLeft = pos.x - rect.left;
        float dxRight = rect.right - pos.x;
        float dyBottom = pos.y - rect.bottom;
        float dyTop = rect.top - pos.y;

        // 选择最近边
        if (dxLeft <= dxRight && dxLeft <= dyBottom && dxLeft <= dyTop)
        {
            impactNormal = Vector2D(-1, 0);
            penetration = radius + dxLeft;
            impactPoint = Vector2D(rect.left, pos.y);
        }
        else if (dxRight <= dxLeft && dxRight <= dyBottom && dxRight <= dyTop)
        {
            impactNormal = Vector2D(1, 0);
            penetration = radius + dxRight;
            impactPoint = Vector2D(rect.right, pos.y);
        }
        else if (dyBottom <= dxLeft && dyBottom <= dxRight && dyBottom <= dyTop)
        {
            impactNormal = Vector2D(0, -1);
            penetration = radius + dyBottom;
            impactPoint = Vector2D(pos.x, rect.bottom);
        }
        else // dyTop 最小
        {
            impactNormal = Vector2D(0, 1);
            penetration = radius + dyTop;
            impactPoint = Vector2D(pos.x, rect.top);
        }
    }
    else
    {
        // 圆心在矩形外部的情况，计算最近点和 penetration
        if (pos.x < rect.left)
        {
            if (pos.y > rect.top)
            {
                impactPoint = { rect.left,rect.top };  impactNormal = (impactPoint - circle->GetWorldPosition()).Normalize();
                penetration = radius - (circle->GetWorldPosition() - Vector2D(rect.left, rect.top)).Size();
            }
            else if (pos.y < rect.bottom)
            {
                impactPoint = { rect.left,rect.bottom };  impactNormal = (impactPoint - circle->GetWorldPosition()).Normalize();
                penetration = radius - (circle->GetWorldPosition() - Vector2D(rect.left, rect.bottom)).Size();
            }
            else {
                impactPoint = { rect.left,pos.y };  impactNormal = { 1,0 }; penetration = radius - (pos.x - rect.left);
            }
        }
        else if (pos.x > rect.right)
        {
            if (pos.y > rect.top)
            {
                impactPoint = { rect.right,rect.top };  impactNormal = (impactPoint - circle->GetWorldPosition()).Normalize();
                penetration = radius - (circle->GetWorldPosition() - Vector2D(rect.right, rect.top)).Size();
            }
            else if (pos.y < rect.bottom)
            {
                impactPoint = { rect.right,rect.bottom };  impactNormal = (impactPoint - circle->GetWorldPosition()).Normalize();
                penetration = radius - (circle->GetWorldPosition() - Vector2D(rect.right, rect.bottom)).Size();
            }
            else {
                impactPoint = { rect.right,pos.y }; impactNormal = { -1,0 }; penetration = radius - (pos.x - rect.right);
            }
        }
        else
        {
            if (pos.y > rect.top) {
                impactPoint = { pos.x,rect.top }; impactNormal = { 0,-1 }; penetration = radius - (pos.y - rect.top);
            }
            else {
                impactPoint = { pos.x,rect.bottom }; impactNormal = { 0,1 }; penetration = radius - (rect.bottom - pos.y);
            }
        }
    }

    // 旋转回世界空间
    impactPoint = Vector2D::RotateVector(angle, impactPoint - boxPos) + boxPos;
    impactNormal = Vector2D::RotateVector(angle, impactNormal);

    // 如果 c1 是圆，impactNormal 保持，否则反向
    return HitResult(impactPoint, impactNormal * (c1 == circle ? 1.f : -1.f), c2->pOwner, c2, penetration);
}

HitResult Collider::collisionHitBoxToBox(Collider* c1, Collider* c2)
{
    
    BoxCollider* box1 = Cast<BoxCollider>(c1);
    BoxCollider* box2 = Cast<BoxCollider>(c2);
    std::vector<Vector2D> verts1 = box1->GetWorldVertices();
    std::vector<Vector2D> verts2 = box2->GetWorldVertices();

    // --- 1. 构建分离轴（矩形边法线） ---
    std::vector<Vector2D> axes;
    for (int i = 0; i < 4; i++)
    {
        Vector2D edge = verts1[(i + 1) % 4] - verts1[i];
        Vector2D normal(-edge.y, edge.x); // 垂直向量
        axes.push_back(normal.Normalize());
    }
    for (int i = 0; i < 4; i++)
    {
        Vector2D edge = verts2[(i + 1) % 4] - verts2[i];
        Vector2D normal(-edge.y, edge.x);
        axes.push_back(normal.Normalize());
    }

    // --- 2. 检查每个轴的投影重叠 ---
    float minOverlap = FLT_MAX;
    Vector2D smallestAxis;
    bool collided = true;

    for (auto& axis : axes)
    {
        float min1, max1, min2, max2;
        BoxCollider::ProjectPolygon(axis, verts1, min1, max1);
        BoxCollider::ProjectPolygon(axis, verts2, min2, max2);

        // 检查是否分离
        if (max1 < min2 || max2 < min1)
        {
            collided = false;
            break; // 找到分离轴，无碰撞
        }

        // 重叠量
        float overlap = min(max1, max2) - max(min1, min2);
        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axis;
            // 确保法线指向 box2 → box1
            Vector2D centerDir = box2->GetWorldPosition() - box1->GetWorldPosition();
            if (Vector2D::DotProduct(centerDir, smallestAxis) < 0)
                smallestAxis *= -1;
        }
    }

    if (!collided)
        return HitResult(); // 没有碰撞

    // --- 3. 计算碰撞点 ---
    // 简单做法：重叠区域中心 = 两矩形中心沿最小轴投影的中点
    Vector2D center1 = box1->GetWorldPosition();
    Vector2D center2 = box2->GetWorldPosition();
    Vector2D impactPoint = (center1 + center2) / 2.0f;

    return HitResult(impactPoint, smallestAxis, box2->pOwner, box2, minOverlap);
}

void Collider::SetLocalPosition(const Vector2D& position)
{
    SceneComponent::SetLocalPosition(position);
	Rcenter = GetWorldPosition();
}

bool Collider::CollisionJudge(Collider* another)
{
    int shape1 = int(this->shape), shape2 = int(another->shape);
    return collisionJudgeMap[shape1 * shape1 + shape2 * shape2](this, another);
}
bool Collider::collisionJudgeCircleToCircle(Collider* c1, Collider* c2)
{
    CircleCollider* circle1 = Cast<CircleCollider>(c1);
    CircleCollider* circle2 = Cast<CircleCollider>(c2);
    return Vector2D::Distance(circle1->GetWorldPosition(), circle2->GetWorldPosition()) <= circle1->GetRadius() + circle2->GetRadius();
}

bool Collider::collisionJudgeCircleToBox(Collider* c1, Collider* c2)
{
    CircleCollider* circle; BoxCollider* box;
    if (c1->GetShape() == Circle)
    {
        circle = Cast<CircleCollider>(c1), box = Cast<BoxCollider>(c2);
    }
    else
    {
        circle = Cast<CircleCollider>(c2), box = Cast<BoxCollider>(c1);
    }
    float radius = circle->GetRadius(); Vector2D pos = circle->GetWorldPosition();
	Vector2D boxPos = box->GetWorldPosition();
    float angle = box->GetWorldRotation();        // 角度
	Vector2D d = pos - boxPos; 
    pos = boxPos + Vector2D::RotateVector(-angle, d);
    BoxCollider::Rect rect = box->GetRect();

    if (pos.x <= rect.right && pos.x >= rect.left && pos.y <= rect.top && pos.y >= rect.bottom)
        return true;
    else
    {
        if (pos.x < rect.left)
        {
            if (pos.y > rect.top)return Vector2D::Distance(pos, { rect.left,rect.top }) <= radius;
            else if (pos.y < rect.bottom)return Vector2D::Distance(pos, { rect.left,rect.bottom }) <= radius;
            else return rect.left - pos.x <= radius;
        }
        else if (pos.x > rect.right)
        {
            if (pos.y > rect.top)return Vector2D::Distance(pos, { rect.right,rect.top }) <= radius;
            else if (pos.y < rect.bottom)return Vector2D::Distance(pos, { rect.right,rect.bottom }) <= radius;
            else return pos.x - rect.right <= radius;
        }
        else
        {
            if (pos.y > rect.top)return pos.y - rect.top <= radius;
            else return rect.bottom - pos.y <= radius;
        }
    }
}

bool Collider::collisionJudgeBoxToBox(Collider* c1, Collider* c2)
{
    BoxCollider* box1 = Cast<BoxCollider>(c1);
    BoxCollider* box2 = Cast<BoxCollider>(c2);
    std::vector<Vector2D> verts1 = box1->GetWorldVertices();
    std::vector<Vector2D> verts2 = box2->GetWorldVertices();

    // 需要检查的4个轴（两个矩形的边方向法线）
    std::vector<Vector2D> axes;
    for (int i = 0; i < 4; i++) {
        Vector2D edge = verts1[(i + 1) % 4] - verts1[i];
        Vector2D axis = edge.Normalize(); 
        axes.push_back(Vector2D(-axis.y, axis.x)); // 法线
    }
    for (int i = 0; i < 4; i++) {
        Vector2D edge = verts2[(i + 1) % 4] - verts2[i];
        Vector2D axis = edge.Normalize();
        axes.push_back(Vector2D(-axis.y, axis.x));
    }

    // 在所有轴上做投影检查
    for (auto& axis : axes) {
        float min1, max1, min2, max2;
        BoxCollider::ProjectPolygon(axis, verts1, min1, max1);
        BoxCollider::ProjectPolygon(axis, verts2, min2, max2);

        if (max1 < min2 || max2 < min1) {
            return false; // 找到分离轴 → 不碰撞
        }
    }

    return true; // 所有轴都有重叠 → 碰撞
}

