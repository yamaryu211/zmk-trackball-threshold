# zmk-trackball-threshold モジュール 最終要件定義

## 1. プロジェクト概要

- **モジュール名**: `zmk-trackball-threshold`
- **目的**: ZMK 標準 input processor による AML 制御において、AML 状態に応じた動的な閾値制御を実現し、意図しない軽微な動きによる AML 誤発動を防止
- **開発方針**: zmk-mouse-gesture の実装パターンを参考とした独立モジュール

## 2. 機能仕様

### 2.1 Input Processor 仕様

- **Compatible**: `"zmk,input-processor-trackball-threshold"`
- **Cells**: `#input-processor-cells = <0>`
- **閾値計算方式**: `abs(x) + abs(y) > threshold`
- **動作**: AML 状態に応じた動的制御

### 2.2 動的制御仕様

#### AML 未発動時

- **閾値以下の入力**: 破棄（AML 発動を抑制）
- **閾値超過の入力**: 無変更で転送（AML 発動を許可）

#### AML 発動後

- **全ての入力**: 無変更で転送（通常のマウス操作を維持）

### 2.3 設定方式

**Device Tree 設定（.keymap ファイル）**:

```c
zip_trackball_threshold: zip_trackball_threshold {
    compatible = "zmk,input-processor-trackball-threshold";
    movement-threshold = <5>; // 省略時はKconfigデフォルト値
};

&trackball_listener {
    input-processors = <&zip_trackball_threshold>, <&zip_temp_layer AML 30000>;
};
```

**Kconfig 設定（.conf ファイル）**:

```c
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD=y
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD_DEFAULT_THRESHOLD=5
```

## 3. 技術仕様

### 3.1 モジュール構造

```
zmk-trackball-threshold/
├── CMakeLists.txt
├── Kconfig
├── README.md
├── development.md
├── dts/
│   └── bindings/
│       └── zmk,input-processor-trackball-threshold.yaml
├── include/
│   └── zmk/
│       └── trackball_threshold.h
├── src/
│   └── trackball_threshold.c
└── zephyr/
    └── module.yml
```

### 3.2 パラメータ仕様

- **デフォルト閾値**: `5`
- **設定可能範囲**: `0`以上の整数値
- **適用範囲**: `trackball_listener`のみ
- **AML レイヤー番号**: `8` (ハードコード)

### 3.3 依存関係

- **必要ヘッダー**: `#include <zmk/keymap.h>`
- **必要 API**: `zmk_keymap_layer_active()`
- **配置順序**: `zip_temp_layer`より前に配置必須

## 4. 実装要件

### 4.1 必須機能

- トラックボール入力の移動量計算
- AML レイヤー状態の検知
- AML 状態に応じた動的制御ロジック
- 閾値との比較判定
- Device Tree プロパティからの設定読み込み

### 4.2 制御ロジック

```c
// AML状態チェック
bool aml_active = zmk_keymap_layer_active(8);

if (!aml_active && movement_magnitude <= threshold) {
    // AML未発動 + 閾値以下 → 破棄
    return 0;
}
// その他の場合 → 転送
```

### 4.3 非機能要件

- **パフォーマンス**: 入力レスポンスに影響しない軽量実装
- **互換性**: 既存 input processor chain との完全互換
- **保守性**: zmk-mouse-gesture と同様の構造
- **信頼性**: AML 状態検知の確実性

## 5. 開発制約

- 既存の zmk-config-roBa には変更を加えない
- 既存の zmk-pmw3610-driver には変更を加えない
- west.yml 設定は後段階で実施
- 独立したモジュールとして完成させる
- AML レイヤー番号は 8 で固定

## 6. 期待効果

### 6.1 ユーザー体験の改善

- **誤発動防止**: 軽微な手ブレで AML が発動しない
- **意図的操作**: 明確な動きで AML が適切に発動
- **操作継続性**: AML 発動後は通常通りの精密制御

### 6.2 実用性向上

- **設定柔軟性**: 閾値調整による感度カスタマイズ
- **透明性**: 既存設定への影響なし
- **拡張性**: 将来的な機能追加への対応

## 7. 開発完了条件

1. AML 状態検知機能の実装完了
2. 動的制御ロジックの実装完了
3. 既存機能との互換性確認完了
4. ドキュメント更新完了
5. 動作確認とデバッグ完了

---

**要件定義更新完了 (v2.0)**

AML 状態に応じた動的制御機能により、より高度な誤発動防止とユーザビリティ向上を実現。
