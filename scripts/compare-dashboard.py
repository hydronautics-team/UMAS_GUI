#!/usr/bin/env python3
"""Generate visual comparison artifacts for the UMAS dashboard reference."""

import argparse
from pathlib import Path

from PIL import Image, ImageChops, ImageFilter, ImageOps, ImageStat


def edge_mask(image: Image.Image) -> Image.Image:
    edges = ImageOps.autocontrast(image.convert("L").filter(ImageFilter.FIND_EDGES))
    return edges.point(lambda pixel: 255 if pixel > 55 else 0)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--output-dir", type=Path, default=Path("build/dashboard-comparison"))
    args = parser.parse_args()

    reference = Image.open(args.reference).convert("RGB")
    candidate = Image.open(args.candidate).convert("RGB")
    if reference.size != candidate.size:
        raise SystemExit(
            f"Image sizes differ: reference={reference.size}, candidate={candidate.size}"
        )

    args.output_dir.mkdir(parents=True, exist_ok=True)
    Image.blend(reference, candidate, 0.5).save(args.output_dir / "overlay.png")

    reference_edges = edge_mask(reference)
    candidate_edges = edge_mask(candidate)
    empty_channel = Image.new("L", reference.size)
    Image.merge("RGB", (reference_edges, candidate_edges, empty_channel)).save(
        args.output_dir / "edges.png"
    )

    difference = ImageChops.difference(reference, candidate)
    mean_absolute_difference = sum(ImageStat.Stat(difference).mean) / 3.0
    print(f"Compared {reference.size[0]}x{reference.size[1]} images")
    print(f"Mean absolute pixel difference: {mean_absolute_difference:.2f}")
    print(f"Overlay: {args.output_dir / 'overlay.png'}")
    print(f"Edges (reference=red, candidate=green): {args.output_dir / 'edges.png'}")


if __name__ == "__main__":
    main()
