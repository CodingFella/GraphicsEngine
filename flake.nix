{
  inputs.nixpkgs.url = "nixpkgs-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    lib = nixpkgs.lib;
    systems = ["aarch64-linux" "x86_64-linux"];
    # packagers = map (system: import nixpkgs {inherit system;}) systems;
    eachSystem = f:
      lib.foldAttrs lib.mergeAttrs {}
      (map (
          s:
            lib.mapAttrs (_: v: {${s} = v;}) (
              f (import nixpkgs {system = s;}) s
            )
        )
        systems);
  in
    eachSystem (pkgs: system: {
      formatter = pkgs.alejandra;

      packages.default = pkgs.stdenv.mkDerivation {
        pname = "graphi";
        version = "0.1.0";

        src = ./.;

        buildInputs = [];

        nativeBuildInputs = [pkgs.zig.hook];
        # zigBuildFlags = [ "--system" "${ callPackage ./neomacs-zig-zon.nix { } }" ];
      };
    });
}
