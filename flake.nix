{
  description = "Rice Engine — C++20 / Vulkan game engine";

  inputs = {
    # Single pinned nixpkgs. flake.lock freezes the exact revision, so the tree
    # still builds years later.
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

    # Third-party sources that nixpkgs does not carry. These replace the old
    # git submodules: the revision is pinned in flake.lock instead of .gitmodules.
    libcds-src = {
      url = "github:khizmax/libcds";
      flake = false;
    };
    rice-meta-compiler-src = {
      url = "github:dgudim/RiceMetaCompiler";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, libcds-src, rice-meta-compiler-src }:
    let
      inherit (nixpkgs) lib;

      systems = [ "x86_64-linux" "aarch64-linux" ];

      # Dependencies that are built from upstream sources instead of vendored
      # headers/binaries. Pinned by flake.lock.
      depsOverlay = final: prev: {
        libcds = prev.stdenv.mkDerivation {
          pname = "libcds";
          version = "2.3.3";
          src = libcds-src;
          nativeBuildInputs = [ prev.cmake ];
          cmakeFlags = [
            # libcds declares cmake_minimum_required(3.0.2); CMake 4 dropped that range.
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
            "-DLIBCDS_WITH_TESTS=OFF"
            "-DLIBCDS_ENABLE_UNIT_TEST=OFF"
            "-DLIBCDS_ENABLE_STRESS_TEST=OFF"
          ];
        };

        rice-meta-compiler = prev.stdenv.mkDerivation {
          pname = "rice-meta-compiler";
          version = "unstable-2022-10-19";
          src = rice-meta-compiler-src;
          nativeBuildInputs = [ prev.cmake ];
          buildInputs = [ prev.nlohmann_json ];
        };
      };

      pkgsFor = system: import nixpkgs {
        inherit system;
        overlays = [ depsOverlay self.overlays.default ];
        # vscode in nixpkgs is the proprietary binary build, and most editor
        # extensions are marked unfree; both are only used by the `vscode` shell.
        config.allowUnfreePredicate = pkg:
          let name = lib.getName pkg; in
          name == "vscode" || lib.hasPrefix "vscode-extension" name;
      };

      eachSystem = f: lib.genAttrs systems (system: f (pkgsFor system));
    in
    {
      overlays.deps = depsOverlay;

      overlays.default = final: prev: {
        rice-engine = final.callPackage ./nix/package.nix { };
      };

      packages = eachSystem (pkgs: {
        default = pkgs.rice-engine;
        inherit (pkgs) rice-engine libcds rice-meta-compiler;
      });

      devShells = eachSystem (pkgs:
        let
          # Environment shared by every shell: toolchain + all build deps, no
          # vendored copies anywhere.
          common = {
            buildInputs = with pkgs; [
              vulkan-headers
              vulkan-loader
              vulkan-validation-layers
              vk-bootstrap
              SDL2
              fmt
              magic-enum
              nlohmann_json
              libogg
              libvorbis
              libcds
              rice-meta-compiler
            ];
            nativeBuildInputs = with pkgs; [
              cmake
              ninja
              pkg-config
              clang-tools
              gdb
              shaderc
              vulkan-tools
            ];
            shellHook = ''
              export CMAKE_PREFIX_PATH="${pkgs.lib.concatStringsSep ":" [
                pkgs.vulkan-loader
                pkgs.vk-bootstrap
                pkgs.SDL2
                pkgs.fmt
                pkgs.magic-enum
                pkgs.nlohmann_json
                pkgs.libcds
              ]}''${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"
              export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"
              export RICE_METACOMPILER_INCLUDE_DIR="${pkgs.rice-meta-compiler}/include"
              # clangd reads this instead of guessing include paths.
              export CMAKE_EXPORT_COMPILE_COMMANDS=ON
              echo "Rice Engine dev shell — cmake $(cmake --version | head -1 | cut -d' ' -f3), glslc $(glslc --version | head -1 | cut -d' ' -f3)"
            '';
          };

          # Same editor setup the old shell.nix carried: VS Code with clangd,
          # C/C++, CMake tools, hex editor, Nix, GLSL/hlsl highlighting.
          vscodeWithExtensions = pkgs.vscode-with-extensions.override {
            vscodeExtensions = (with pkgs.vscode-extensions; [
              llvm-vs-code-extensions.vscode-clangd
              ms-vscode.cpptools
              ms-vscode.cmake-tools
              ms-vscode.hexeditor
              # tal7aouy.icons was a fork of vscode-icons that stopped at 3.8.0
              # (Dec 2023); upstream is the maintained one.
              vscode-icons-team.vscode-icons
              jnoortheen.nix-ide
              wmaurer.change-case
              oderwat.indent-rainbow
              ibm.output-colorizer
            ]) ++ pkgs.vscode-utils.extensionsFromVscodeMarketplace [
              {
                name = "vscode-glsllint";
                publisher = "dtoplak";
                version = "1.9.1";
                sha256 = "sha256-59RNmILdshkAYjpeNz+gjq+WuDKZ/IMNVdzVl8HLFaA=";
              }
              {
                # hash fixed: the one kept in the old shell.nix was truncated
                name = "shader";
                publisher = "slevesque";
                version = "1.1.5";
                sha256 = "sha256-Pf37FeQMNlv74f7LMz9+CKscF6UjTZ7ZpcaZFKtX2ZM=";
              }
            ];
          };
        in
        {
          default = pkgs.mkShell ({ name = "rice-engine-dev"; } // common);

          # `nix develop .#vscode` (or ./develop.sh) — editor + full toolchain.
          vscode = pkgs.mkShell ({
            name = "rice-engine-vscode";
            packages = [ vscodeWithExtensions ];
          } // common);
        });

      # `nix flake check` — build + headless smoke tests against lavapipe.
      checks = eachSystem (pkgs: import ./nix/checks.nix { inherit pkgs; });

      formatter = eachSystem (pkgs: pkgs.nixpkgs-fmt);
    };
}
