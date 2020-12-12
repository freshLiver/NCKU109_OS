sudo perf script > out.perf
../FlameGraph/stackcollapse-perf.pl out.perf > out.folded
../FlameGraph/flamegraph.pl out.folded > out.svg
rm out.folded
sudo chown freshliver:freshliver out.perf out.svg perf.data*
sudo chmod 666 out.perf out.svg perf.data*
