with (import <nixpkgs> { });
let llvm_packages = llvmPackages_19;
in mkShell {
    inherit (llvm_packages) stdenv;
  buildInputs = [
    nlohmann_json # replace with libyaml
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    vk-bootstrap
    magic-enum
    SDL2
    fmt_11
  ];
  packages = [
    vulkan-tools
    cmake
    ninja
    shaderc
    gdb
    llvm_packages.clang-tools
    gh
    (vscode-with-extensions.override {
      vscodeExtensions = with vscode-extensions;
        [
          llvm-vs-code-extensions.vscode-clangd
          ms-vscode.cpptools
          ms-vscode.cmake-tools
          ms-vscode.hexeditor
          tal7aouy.icons
          jnoortheen.nix-ide
          wmaurer.change-case
          oderwat.indent-rainbow
          ibm.output-colorizer
        ] ++ vscode-utils.extensionsFromVscodeMarketplace [
          {
            name = "vscode-glsllint";
            publisher = "dtoplak";
            version = "1.9.1";
            hash = "sha256-59RNmILdshkAYjpeNz+gjq+WuDKZ/IMNVdzVl8HLFaA=";
          }
          {
            name = "shader";
            publisher = "slevesque";
            version = "1.1.5";
            hash = "sha256-Pf37FeQMNlv74f7LMz9+CKscF6UjTZ7ZpcaZFKtX2ZM=";
          }
        ];
    })
  ];
}
