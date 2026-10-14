"""Validate generated manifests against downloaded official JSON schemas.

Optional developer check: python -m pip install jsonschema
python scripts/validate-schema.py --schema research/qmod.schema.json mod.json
"""
import argparse
import json
from pathlib import Path
import jsonschema

parser = argparse.ArgumentParser()
parser.add_argument("--schema", required=True, type=Path)
parser.add_argument("manifest", type=Path)
parser.add_argument("--legacy-fragment-ids", action="store_true",
                    help="Remove obsolete fragment-only $id annotations from upstream schemas; preserve all constraints")
args = parser.parse_args()
schema = json.loads(args.schema.read_text(encoding="utf-8"))
if args.legacy_fragment_ids:
    def strip_fragment_ids(node):
        if isinstance(node, dict):
            if str(node.get("$id", "")).startswith("#"):
                del node["$id"]
            for value in node.values():
                strip_fragment_ids(value)
        elif isinstance(node, list):
            for value in node:
                strip_fragment_ids(value)
    strip_fragment_ids(schema)
    print("Note: removed legacy fragment-only $id annotations; validation constraints unchanged.")
manifest = json.loads(args.manifest.read_text(encoding="utf-8-sig"))
validator_type = jsonschema.validators.validator_for(schema)
validator_type.check_schema(schema)
validator_type(schema).validate(manifest)
print(f"PASS: {args.manifest} matches {args.schema.name}")
