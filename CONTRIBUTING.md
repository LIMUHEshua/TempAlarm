# Contributing

感谢你对 TempAlarm 项目的关注与贡献。

## 1. 提交 Issue

- 在 GitHub Issues 中描述问题、复现步骤、预期行为和实际行为。
- 如果可能，请附上相关日志、截图或设备信息。
- 对于功能请求，请说明实际使用场景与价值。

## 2. 提交 Pull Request

- 优先从 `main` 分支创建一个功能分支或修复分支
- 在提交前请确保代码可编译
- 保持修改范围聚焦，避免混杂无关变更
- 提交 PR 前请说明改动目的、测试情况和影响范围

## 3. 代码风格要求

- 保持代码简洁、清晰、可维护
- 变量和函数命名尽量具有描述性
- 重要逻辑补充必要注释
- 保持与现有工程风格一致

## 4. 分支策略

- `main` 为稳定分支
- 功能开发和修复应在 `develop` 或单独功能分支中进行
- 提交完成后通过 Pull Request 合并到主分支

## 5. 提交信息规范

建议使用清晰的提交信息，例如：

```bash
git commit -m "feat: add DHT11 sensor sampling"
git commit -m "fix: correct BH1750 I2C timing"
git commit -m "docs: update README and contribution guide"
```

## 6. 额外建议

- 不建议直接修改 `main` 分支
- 提交前先同步远端最新状态
- 代码和文档修改应遵循项目许可要求

## 7. 开源协议

本项目采用 Apache License 2.0。提交代码时，默认视为同意相关开源协议要求。
