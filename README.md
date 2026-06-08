# openGL-CG

Aplicação OpenGL em C++ com sistema de cenas. O projeto reúne exercícios da disciplina (TDE) e o trabalho final: um **quarto gamer** modelado no 3ds Max e renderizado na engine.

## Estrutura do repositório

```
BaseAppOGL/
├── Engine/                 # Motor (câmera, shaders, input, SceneManager, Assimp...)
├── Scenes/
│   ├── Common/             # Shaders compartilhados (grid, axis, texto, etc.)
│   ├── tde/                # Cenas dos exercícios da disciplina
│   └── FinalProject/       # Cena OpenGL do projeto final
├── D3S MAX project/        # Modelagem 3D do quarto gamer (3ds Max)
├── OpenGL_SDK/             # Dependências (GLFW, GLEW, GLM, Assimp...)
└── exercicios/             # Materiais e binários das listas de exercícios
```

---

## Cenas TDE (`Scenes/tde/`)

Exercícios desenvolvidos ao longo da disciplina, acessíveis pelas teclas **1 a 4**.

| Tecla | Pasta | Descrição |
|-------|--------|-----------|
| **1** | `tde/Scene1` | Cena com texturas: chão, caixa, casa e materiais (`Exer3/`). |
| **2** | `tde/Scene2` | Cena com árvore, boneco de neve e shaders de exercício (`exerc3`). |
| **3** | `tde/Scene3` | Simulação de cubos empilhados em pilhas (inserção automática). |
| **4** | `tde/Scene4` | Casa carregada a partir de arquivo de vértices (`Casa.txt`) + texturas. |

Cada cena tem seus próprios shaders, geometria e assets dentro da respectiva pasta.

---

## Projeto final — Quarto gamer

O projeto final é dividido em **duas partes** que se complementam:

### 1. Modelagem no 3ds Max (`D3S MAX project/`)

Ambiente 3D do quarto gamer criado no **Autodesk 3ds Max**:

- `project.max` — arquivo principal da cena no Max
- `materials/` — texturas e materiais usados no quarto
  - Tijolo (`Bricks073A_1K-PNG/`)
  - Piso (`floor/`, `Ash Black Silver/`)

Aqui são definidos chão, paredes, teto, janelas, mobília e materiais. O modelo é exportado (FBX) para ser carregado na engine via **Assimp** (`Engine/CLoadAssets`).

### 2. Cena OpenGL (`Scenes/FinalProject/`)

Implementação em C++ da cena do projeto na engine:

- `FinalProject.h` / `FinalProject.cpp` — classe da cena (câmera, grid, input, render)
- Carregamento e exibição dos modelos exportados do 3ds Max
- Lógica específica do projeto (iluminação, interação, HUD, etc.)

**Tecla `0`** abre esta cena (também é a cena inicial ao iniciar o programa).

### Fluxo de trabalho

```
3ds Max (D3S MAX project/)  →  export FBX  →  FinalProject (OpenGL)
     modelagem + texturas         Assimp           cena + código C++
```

---

## Atalhos globais

| Tecla | Ação |
|-------|------|
| `0` | Cena **FinalProject** (projeto final) |
| `1`–`4` | Cenas **TDE** |
| `W A S D` | Mover câmera |
| `E` / `Q` | Subir / descer |
| `Shift` | Velocidade rápida |
| `Mouse` | Olhar ao redor |
| `Scroll` | Zoom |
| `TAB` | Wireframe on/off |
| `V` | VSync on/off |
| `ESC` | Fechar |

---

## Como compilar

1. Abrir `BaseAppOGL.sln` no Visual Studio
2. Configuração: **Debug | x64**
3. **Build → Rebuild Solution**
4. Executar `bin/Debug/BaseAppOGL.exe`

O SDK OpenGL está em `OpenGL_SDK/OpenGL_SDK` (ou via variável de ambiente `OpenGL_SDK`).

---

## Observações

- As cenas TDE e o projeto final convivem no mesmo executável; o `SceneManager` troca entre elas.
- Modelos do 3ds Max devem ser exportados com **UV/mapping** para as texturas funcionarem corretamente no OpenGL.
- O Max usa **Z-up**; na importação pode ser necessário ajustar rotação/escala na cena `FinalProject`.
