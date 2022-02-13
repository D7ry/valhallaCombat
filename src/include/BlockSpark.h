#pragma once
/*Maxsu's block spark function*/
namespace MaxsuBlockSpark
{
	class SparkLocalizer
	{
		using shieldEdg = std::pair<RE::NiAVObject*, RE::NiPoint3>;

	private:
		template <typename T>
		struct matrix_size;

		template <typename T, size_t N>
		struct matrix_size<T[N][N]>
		{
			typedef T type;
			static size_t const value = N;
		};

		template <typename Array>
		bool InverseMatrix(Array& out, Array const& in_)
		{
			typedef typename matrix_size<Array>::type type;
			size_t const n = matrix_size<Array>::value;
			Array in;
			// 复制输入，并初始化out
			for (size_t i = 0; i != n; ++i) {
				for (size_t j = 0; j != n; ++j) {
					in[i][j] = in_[i][j];
					out[i][j] = 0;
				}
				out[i][i] = 1;	// 将out赋值为单位阵
			}

			for (size_t c = 0; c != n; ++c) {
				// 选取列主元，这样算法更稳定
				size_t rr = 0;
				type maximum = 0;
				for (size_t r = c; r != n; ++r) {
					type const tmp = std::abs(in[r][c]);
					if (tmp > maximum) {
						maximum = tmp;
						rr = r;
					}
				}
				if (maximum == 0)
					return false;  // 不可逆

				// 交换c, rr两行
				for (size_t i = c; i != n; ++i) std::swap(in[c][i], in[rr][i]);
				for (size_t i = 0; i != n; ++i) std::swap(out[c][i], out[rr][i]);

				// 正规化
				for (size_t i = c + 1; i != n; ++i) {
					out[c][i] /= in[c][c];
				}
				for (size_t i = 0; i != n; ++i) {
					out[c][i] /= in[c][c];
				}

				// 行变换消元
				in[c][c] = 0;
				for (size_t r = 0; r != n; ++r) {
					for (size_t i = 0; i != n; ++i) {
						in[r][i] += in[c][i] * -in[r][c];
						out[r][i] += out[c][i] * -in[r][c];
					}
				}
			}

			return true;
		}
	public:

		static SparkLocalizer* GetSingleton()
		{
			static SparkLocalizer singleton;
			return  std::addressof(singleton);
		}
		/*
		bool GetShieldSparkPos(const RE::NiPoint3& hitPos, RE::NiAVObject* shieldNode, RE::NiPoint3& result) {
			{
				if (!shieldNode)
					return false;

				auto GetShieldShapeExtent = [](RE::NiAVObject* shieldNode, RE::NiPoint3& centerPos, float& z_extent, float& y_extent) -> bool {

					RE::bhkNiCollisionObject* shieldCollied = nullptr;
					if (shieldNode->collisionObject)
						shieldCollied = shieldNode->collisionObject->AsBhkNiCollisionObject();
					else {
						auto childNode = shieldNode->GetObjectByName("SHIELD");
						shieldCollied = childNode && childNode->collisionObject ? childNode->collisionObject->AsBhkNiCollisionObject() : nullptr;
					}

					if (!shieldCollied)
						return false;

					RE::bhkRigidBody* bhkShieldRigid = shieldCollied->body.get() ? shieldCollied->body.get()->AsBhkRigidBody() : nullptr;
					RE::hkpRigidBody* hkpShieldRigid = bhkShieldRigid ? skyrim_cast<RE::hkpRigidBody*>(bhkShieldRigid->referencedObject.get()) : nullptr;
					if (bhkShieldRigid && hkpShieldRigid) {

						centerPos = shieldNode->worldBound.center;
						RE::bhkRigidBodyT* bhkShieldRigidT = skyrim_cast<RE::bhkRigidBodyT*>(bhkShieldRigid);
						if (bhkShieldRigidT) {
							float bodyTrans[4];
							_mm_store_ps(bodyTrans, bhkShieldRigidT->translation.quad);
							const auto matrix = shieldNode->world.rotate;
							centerPos += matrix * RE::NiPoint3(bodyTrans[0], bodyTrans[1], bodyTrans[2]) * RE::bhkWorld::GetWorldScaleInverse();
						}

						const RE::hkpShape* shieldShape = hkpShieldRigid->collidable.GetShape();
						if (shieldShape) {
							float upExtent = shieldShape->GetMaximumProjection(RE::hkVector4{ 0.0f,0.0f,1.0f,0.0f }) * RE::bhkWorld::GetWorldScaleInverse();
							float downExtent = shieldShape->GetMaximumProjection(RE::hkVector4{ 0.0f,0.0f,-1.0f,0.0f }) * RE::bhkWorld::GetWorldScaleInverse();
							z_extent = upExtent + downExtent;

							float forwardExtent = shieldShape->GetMaximumProjection(RE::hkVector4{ 0.0f,1.0f,0.0f,0.0f }) * RE::bhkWorld::GetWorldScaleInverse();
							float backwardExtent = shieldShape->GetMaximumProjection(RE::hkVector4{ 0.0f,-1.0f,0.0f,0.0f }) * RE::bhkWorld::GetWorldScaleInverse();
							y_extent = (forwardExtent + backwardExtent) / 2.0f;

							return true;
						}
					}

					return false;
				};

				RE::NiPoint3 centerPos;
				float z_extent, y_extent;

				if (!GetShieldShapeExtent(shieldNode, centerPos, z_extent, y_extent)) {
					INFO("Not Shield Physical Value Found!");
					return false;
				}

				const auto matrix = shieldNode->world.rotate;
				RE::NiMatrix3 invMatrix;
				InverseMatrix(invMatrix.entry, matrix.entry);
				const auto localVector = invMatrix * (hitPos - centerPos);

				result = centerPos + matrix * RE::NiPoint3(0.f, std::clamp(localVector.y, -y_extent * shieldNode->world.scale, y_extent * shieldNode->world.scale), min(-z_extent, -10.f) * shieldNode->world.scale);

				return true;
			}
		}*/


	};

	class blockSpark
	{
	public:

		static blockSpark* GetSingleton()
		{
			static blockSpark singleton;
			return  std::addressof(singleton);
		}

		void playPerfectBlockSpark(RE::Actor* attacker, RE::Actor* defender) {
			auto attackWeapon = attacker->GetAttackingWeapon()->object->As<RE::TESObjectWEAP>();
			if (!attackWeapon) {
				return;
			}
			if (!defender || !attackWeapon || !defender->currentProcess || !defender->currentProcess->high || !attackWeapon->IsMelee() || attackWeapon->IsHandToHandMelee() || !defender->Get3D()) {
				return;
			}
			if (!attacker || !attacker->currentProcess || !attacker->currentProcess->high) {
				return;
			}
			auto attackerData = attacker->currentProcess->high->attackData;
			if (!attackerData) {
				return;
			}
			auto attackerNode = attackerData->IsLeftAttack() ? attacker->GetNodeByName("SHIELD") : attacker->GetNodeByName("WEAPON");
			if (!attackerNode) {
				return;
			}
			auto GetBipeObjIndex = [](RE::TESForm* parryEquipment, bool rightHand) -> RE::BIPED_OBJECT {
				if (!parryEquipment)
					return RE::BIPED_OBJECT::kNone;

				if (parryEquipment->As<RE::TESObjectWEAP>()) {
					switch (parryEquipment->As<RE::TESObjectWEAP>()->GetWeaponType()) {
					case RE::WEAPON_TYPE::kOneHandSword:
						return rightHand ? RE::BIPED_OBJECT::kOneHandSword : RE::BIPED_OBJECT::kShield;

					case RE::WEAPON_TYPE::kOneHandAxe:
						return rightHand ? RE::BIPED_OBJECT::kOneHandAxe : RE::BIPED_OBJECT::kShield;

					case RE::WEAPON_TYPE::kOneHandMace:
						return rightHand ? RE::BIPED_OBJECT::kOneHandMace : RE::BIPED_OBJECT::kShield;

					case RE::WEAPON_TYPE::kTwoHandAxe:
					case RE::WEAPON_TYPE::kTwoHandSword:
						return RE::BIPED_OBJECT::kTwoHandMelee;
					}
				}
				else if (parryEquipment->IsArmor())
					return RE::BIPED_OBJECT::kShield;

				return RE::BIPED_OBJECT::kNone;
			};
			RE::BIPED_OBJECT BipeObjIndex;
			auto defenderLeftEquipped = defender->currentProcess->GetEquippedLeftHand();
			auto defenderData = defender->currentProcess->high->attackData;

			if (defenderData)	//To compatible with "Simple Weapon Swing Parry" while attacking.
				BipeObjIndex = defenderData->IsLeftAttack() && defenderLeftEquipped ? GetBipeObjIndex(defenderLeftEquipped, false) : GetBipeObjIndex(defender->currentProcess->GetEquippedRightHand(), true);
			else
				BipeObjIndex = defenderLeftEquipped && (defenderLeftEquipped->IsWeapon() || defenderLeftEquipped->IsArmor()) ? GetBipeObjIndex(defenderLeftEquipped, false) : GetBipeObjIndex(defender->currentProcess->GetEquippedRightHand(), true);

			if (BipeObjIndex == RE::BIPED_OBJECT::kNone) {
				return;
			}

			auto defenderNode = defender->GetCurrentBiped()->objects[BipeObjIndex].partClone;
			if (!defenderNode || !defenderNode.get()) {
				return;
			}

			auto cell = defender->GetParentCell();

			const auto modelName = BipeObjIndex == RE::BIPED_OBJECT::kShield && defenderLeftEquipped && defenderLeftEquipped->IsArmor() ? "ValhallaCombat\\fxmetalsparkimpactshield.nif" : "ValhallaCombat\\fxmetalsparkimpactweap.nif";

			RE::NiPoint3 sparkPos;
			
			//FIXME: this is bugged
			//RE::NiPoint3 hitPos = attackerNode->worldBound.center + attackerNode->world.rotate * RE::NiPoint3(0.f, 0.5f * attackerNode->worldBound.radius, 0.f);
			/*if (BipeObjIndex == RE::BIPED_OBJECT::kShield && defenderLeftEquipped && defenderLeftEquipped->IsArmor() && SparkLocalizer::GetSingleton()->GetShieldSparkPos(hitPos, defenderNode.get(), sparkPos)) {
				DEBUG("Get Shield Spark Position!");
			}*/
			sparkPos = defenderNode->worldBound.center;
			DEBUG("Get Weapon Spark Position!");
			if (cell->PlaceParticleEffect(0.0f, modelName, defenderNode->world.rotate, sparkPos, 1.0f, 4U, defenderNode.get())) {
				DEBUG("Play Spark Effect Successfully!");
			}

		}
	};
}

