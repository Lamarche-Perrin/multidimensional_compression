# Multidimensional Compression

This program implements a combinatorial algorithm for the lossy
compression of multidimensional datasets by computing optimal rectangular
partitions. A trade-off between the size of the resulting partition and
the information loss it induces is optimised by dynamic programming. In
addition, feasibility constraints can be express on the subsets that might
appear in such partitions. For details regarding the formal grounds of this
work, please refer to:

Robin Lamarche-Perrin. An Information-theoretic Framework for the Lossy
Compression of Link Streams. Research Report, arXiv:1807.06874, July 2018.
<https://arxiv.org/abs/1807.06874>


## Clone and Compile

```
git clone https://github.com/Lamarche-Perrin/multidimensional_compression.git
cd multidimensional_compression
g++ -Wall -std=c++11 multidimensional_compression.cpp -o multidimensional_compression
```

## License

Copyright © 2018 Robin Lamarche-Perrin
(<Robin.Lamarche-Perrin@lip6.fr>)

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.

