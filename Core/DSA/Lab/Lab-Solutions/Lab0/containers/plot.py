import matplotlib.pyplot as plt
import sys
from collections import defaultdict

# Plots the growth curves produced by race for the drift_runner log:
# how fast each candidate container logs new file IDs, how fast it can
# later confirm whether a given file ID was ever logged via std::find,
# and (for set / unordered_set only) how fast the member .find() is.

if len(sys.argv) < 2:
    print('Usage: python3 plot.py <output_file>')
    sys.exit(1)

filename = sys.argv[1]

# container name -> (sizes, insert_us, find_std_us, find_member_us)
data = defaultdict(lambda: ([], [], [], []))

with open(filename, 'r') as datafile:
    for line in datafile:
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        name = parts[0]
        size = int(parts[1])
        insert_us = float(parts[2])
        find_std_us = float(parts[3])
        find_member_us = float(parts[4])

        sizes, inserts, find_std, find_member = data[name]
        sizes.append(size)
        inserts.append(insert_us)
        find_std.append(find_std_us)
        find_member.append(find_member_us)

fig, (ax_insert, ax_find, ax_member) = plt.subplots(1, 3, figsize=(18, 5))

for name, (sizes, inserts, find_std, find_member) in data.items():
    ax_insert.plot(sizes, inserts, marker='o', label=name)
    ax_find.plot(sizes, find_std, marker='o', label=name)

    # member .find() only exists for set / unordered_set. Skip the
    # sentinel -1 values used by vector / list, and give it its own
    # axes so it isn't flattened by the much larger std::find times.
    if all(v >= 0 for v in find_member):
        ax_member.plot(sizes, find_member, marker='s', label=name)

ax_insert.set_title('Logging file IDs vs. log size\n(how fast can each container grow the drift_runner log?)')
ax_insert.set_xlabel('n (file IDs logged)')
ax_insert.set_ylabel('Average time (microseconds)')
ax_insert.legend()

ax_find.set_title('Checking file IDs vs. log size\n(std::find, all containers)')
ax_find.set_xlabel('n (file IDs checked)')
ax_find.set_ylabel('Average time (microseconds)')
ax_find.legend()

ax_member.set_title('Checking file IDs vs. log size\n(member .find(), set / unordered_set only)')
ax_member.set_xlabel('n (file IDs checked)')
ax_member.set_ylabel('Average time (microseconds)')
ax_member.legend()

plt.tight_layout()
out_path = filename.rsplit('.', 1)[0] + '_plot.png'
plt.savefig(out_path, dpi=120)
print(f'Saved plot to {out_path}')

try:
    plt.show()
except Exception:
    pass