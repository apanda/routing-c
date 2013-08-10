import igraph
g = igraph.Graph.Read_GML('a.gml')
g.write_svg('a.svg')
