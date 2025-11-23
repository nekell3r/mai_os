#!/usr/bin/env python3
"""
Script to build graphs from performance_results.csv for Lab2 report
Usage: python build_graphs.py
"""

import pandas as pd
import matplotlib.pyplot as plt
import os

# Check if CSV file exists
if not os.path.exists('performance_results.csv'):
    print("Error: performance_results.csv not found!")
    print("Please run test_performance.ps1 first")
    exit(1)

# Read CSV (handle comma as decimal separator)
df = pd.read_csv('performance_results.csv', decimal=',')
print(f"Loaded {len(df)} records")
print(df)

# Create img directory if it doesn't exist
img_dir = '../report/img'
os.makedirs(img_dir, exist_ok=True)

# Set style
plt.style.use('seaborn-v0_8-darkgrid')
colors = ['#2E86AB', '#A23B72', '#F18F01']

# Graph 1: Execution time vs Number of threads
plt.figure(figsize=(10, 6))
for i, size in enumerate(sorted(df['Size'].unique())):
    data = df[df['Size'] == size].sort_values('Threads')
    plt.plot(data['Threads'], data['AvgTime'], 
             marker='o', linewidth=2, markersize=8,
             color=colors[i % len(colors)],
             label=f'{size}×{size}')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Execution Time (ms)', fontsize=12)
plt.title('Execution Time vs Number of Threads', fontsize=14, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(f'{img_dir}/execution_time.png', dpi=300, bbox_inches='tight')
print(f"✓ Saved {img_dir}/execution_time.png")

# Graph 2: Speedup vs Number of threads
plt.figure(figsize=(10, 6))
for i, size in enumerate(sorted(df['Size'].unique())):
    data = df[df['Size'] == size].sort_values('Threads')
    plt.plot(data['Threads'], data['Speedup'], 
             marker='o', linewidth=2, markersize=8,
             color=colors[i % len(colors)],
             label=f'{size}×{size}')

# Ideal speedup line
max_threads = df['Threads'].max()
plt.plot([1, max_threads], [1, max_threads], 
         'k--', linewidth=2, alpha=0.5, label='Ideal Speedup')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup vs Number of Threads', fontsize=14, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(f'{img_dir}/speedup.png', dpi=300, bbox_inches='tight')
print(f"✓ Saved {img_dir}/speedup.png")

# Graph 3: Efficiency vs Number of threads
plt.figure(figsize=(10, 6))
for i, size in enumerate(sorted(df['Size'].unique())):
    data = df[df['Size'] == size].sort_values('Threads')
    efficiency_percent = data['Efficiency'] * 100
    plt.plot(data['Threads'], efficiency_percent, 
             marker='o', linewidth=2, markersize=8,
             color=colors[i % len(colors)],
             label=f'{size}×{size}')

# 100% efficiency line
plt.axhline(y=100, color='k', linestyle='--', linewidth=2, alpha=0.5, label='100% Efficiency')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Efficiency (%)', fontsize=12)
plt.title('Efficiency vs Number of Threads', fontsize=14, fontweight='bold')
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.ylim(0, 110)
plt.tight_layout()
plt.savefig(f'{img_dir}/efficiency.png', dpi=300, bbox_inches='tight')
print(f"✓ Saved {img_dir}/efficiency.png")

# Graph 4: Combined view for largest matrix
largest_size = df['Size'].max()
data = df[df['Size'] == largest_size].sort_values('Threads')

fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(15, 4))

# Time
ax1.plot(data['Threads'], data['AvgTime'], marker='o', linewidth=2, markersize=8, color=colors[0])
ax1.set_xlabel('Threads')
ax1.set_ylabel('Time (ms)')
ax1.set_title(f'Execution Time\n({largest_size}×{largest_size})')
ax1.grid(True, alpha=0.3)

# Speedup
ax2.plot(data['Threads'], data['Speedup'], marker='o', linewidth=2, markersize=8, color=colors[1])
ax2.plot([1, max_threads], [1, max_threads], 'k--', alpha=0.5, label='Ideal')
ax2.set_xlabel('Threads')
ax2.set_ylabel('Speedup')
ax2.set_title(f'Speedup\n({largest_size}×{largest_size})')
ax2.legend()
ax2.grid(True, alpha=0.3)

# Efficiency
ax3.plot(data['Threads'], data['Efficiency'] * 100, marker='o', linewidth=2, markersize=8, color=colors[2])
ax3.axhline(y=100, color='k', linestyle='--', alpha=0.5)
ax3.set_xlabel('Threads')
ax3.set_ylabel('Efficiency (%)')
ax3.set_title(f'Efficiency\n({largest_size}×{largest_size})')
ax3.grid(True, alpha=0.3)
ax3.set_ylim(0, 110)

plt.tight_layout()
plt.savefig(f'{img_dir}/combined.png', dpi=300, bbox_inches='tight')
print(f"✓ Saved {img_dir}/combined.png")

print("\n✅ All graphs generated successfully!")
print(f"\nGraphs saved to: {os.path.abspath(img_dir)}/")
print("\nGenerated files:")
print("  - execution_time.png")
print("  - speedup.png")
print("  - efficiency.png")
print("  - combined.png")
print("\nYou can now use these images in your LaTeX report!")

# Print summary statistics
print("\n" + "="*60)
print("SUMMARY STATISTICS")
print("="*60)

for size in sorted(df['Size'].unique()):
    print(f"\nMatrix size: {size}×{size}")
    data = df[df['Size'] == size].sort_values('Threads')
    print(f"{'Threads':<10} {'Time (ms)':<12} {'Speedup':<10} {'Efficiency'}")
    print("-" * 50)
    for _, row in data.iterrows():
        threads = int(row['Threads'])
        avg_time = float(row['AvgTime'])
        speedup = float(row['Speedup'])
        efficiency = float(row['Efficiency'])
        print(f"{threads:<10} {avg_time:<12.2f} {speedup:<10.2f} {efficiency*100:.1f}%")

# Find optimal thread count
print("\n" + "="*60)
print("OPTIMAL THREAD COUNT (Efficiency > 70%)")
print("="*60)
for size in sorted(df['Size'].unique()):
    data = df[(df['Size'] == size) & (df['Efficiency'] > 0.7)].sort_values('Threads', ascending=False)
    if not data.empty:
        optimal = data.iloc[0]
        print(f"Matrix {size}×{size}: {optimal['Threads']} threads (Efficiency: {optimal['Efficiency']*100:.1f}%)")
    else:
        print(f"Matrix {size}×{size}: No configuration with efficiency > 70%")

