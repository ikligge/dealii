//----------------------------  data_out_base.cc  ---------------------------
//    $Id$
//    Version: $Name$
//
//    Copyright (C) 1998, 1999, 2000 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------  data_out_base.cc  ---------------------------


#include \"data.inc\" " << endl;
      else                          // all definitions in data file
	{  
                                    // camera
	  out << endl << endl
	      << "camera {"            << endl
	      << "  location <1,4,-7>" << endl
	      << "  look_at <0,0,0>"   << endl
	      << "  angle 30"          << endl
	      << "}"                   << endl;
      
                                    // light
	  out << endl 
	      << "light_source {"      << endl
	      << "  <1,4,-7>"	   << endl
	      << "  color Grey"        << endl
	      << "}"                   << endl;
	  out << endl 
	      << "light_source {"      << endl
	      << "  <0,20,0>"	   << endl
	      << "  color White"       << endl
	      << "}"                   << endl;
	}
    };


double hmin=0,hmax=0;                             // max. and min. heigth of solution 

  for (typename vector<Patch<dim> >::const_iterator patch=patches.begin();
       patch != patches.end(); ++patch)
    {
      const unsigned int n_subdivisions = patch->n_subdivisions;
      
      Assert (patch->data.m() == n_data_sets,
	      ExcUnexpectedNumberOfDatasets (patch->data.m(), n_data_sets));
      Assert (patch->data.n() == (dim==1 ?
				  n_subdivisions+1 :
				  (dim==2 ?
				   (n_subdivisions+1)*(n_subdivisions+1) :
				   (dim==3 ?
				    (n_subdivisions+1)*(n_subdivisions+1)*(n_subdivisions+1) :
				    0))),
	      ExcInvalidDatasetSize (patch->data.n(), n_subdivisions+1));
      
      for (unsigned int i=0; i<n_subdivisions; ++i)
	for (unsigned int j=0; j<n_subdivisions; ++j)
	  {
	    const int dl = i*(n_subdivisions+1)+j;
	    if ((hmin==0)||(patch->data(0,dl)<hmin)) hmin=patch->data(0,dl);
	    if ((hmax==0)||(patch->data(0,dl)>hmax)) hmax=patch->data(0,dl);
	  }
    }

  out << "#declare HMIN=" << hmin << ";" << endl
      << "#declare HMAX=" << hmax << ";" << endl << endl;

  if (!flags.external_data)
    {
                                    // texture with scaled niveau lines
                                    // 10 lines in the surface
      out << "#declare Tex=texture{" << endl
	  << "  pigment {" << endl
	  << "    gradient y" << endl
	  << "    scale y*(HMAX-HMIN)*" << 0.1 << endl
	  << "    color_map {" << endl
	  << "      [0.00 color Light_Purple] " << endl
	  << "      [0.95 color Light_Purple] " << endl
	  << "      [1.00 color White]    " << endl
	  << "} } }" << endl << endl;
    }

  if (!flags.bicubic_patch)
    {                                  // start of mesh header
      out << endl
	  << "mesh {" << endl;
    }

	                              // loop over all patches
  for (typename vector<Patch<dim> >::const_iterator patch=patches.begin();
       patch != patches.end(); ++patch)
    {
      const unsigned int n_subdivisions = patch->n_subdivisions;
      
      Assert (patch->data.m() == n_data_sets,
	      ExcUnexpectedNumberOfDatasets (patch->data.m(), n_data_sets));
      Assert (patch->data.n() == (dim==1 ?
				  n_subdivisions+1 :
				  (dim==2 ?
				   (n_subdivisions+1)*(n_subdivisions+1) :
				   (dim==3 ?
				    (n_subdivisions+1)*(n_subdivisions+1)*(n_subdivisions+1) :
				    0))),
	      ExcInvalidDatasetSize (patch->data.n(), n_subdivisions+1));


Point<dim> ver[16];			    // value for all points in this patch
      
      for (unsigned int i=0; i<n_subdivisions+1; ++i)
	{
	  for (unsigned int j=0; j<n_subdivisions+1; ++j)
	    {
	      const double x_frac = i * 1./n_subdivisions,
		y_frac = j * 1./n_subdivisions;
		                                   // compute coordinates for
		                                   // this patch point, storing in ver
	      ver[i*(n_subdivisions+1)+j]= (((patch->vertices[1] * x_frac) +
					     (patch->vertices[0] * (1-x_frac))) * (1-y_frac) +
					    ((patch->vertices[2] * x_frac) +
					     (patch->vertices[3] * (1-x_frac))) * y_frac);
	    };
	};
      
      if (!flags.bicubic_patch)
	{                                    // setting up triangles
	  for (unsigned int i=0; i<n_subdivisions; ++i)
	    {
	      for (unsigned int j=0; j<n_subdivisions; ++j)
		{
						     // down/left vertex of triangle
		  const int dl = i*(n_subdivisions+1)+j;
		  if (flags.smooth)               // only if smooth triangles are used
		    {
		      Point<3> nrml[16];     // aproximate normal vectors in patch
		      for (unsigned int i=0; i<n_subdivisions+1;++i)
			{
			  for (unsigned int j=0; j<n_subdivisions+1;++j)
			    {
			      if (i==0) 
				nrml[i*(n_subdivisions+1)+j](0)=-patch->data(0,i*(n_subdivisions+1)+j);
			      else 
				nrml[i*(n_subdivisions+1)+j](0)=-patch->data(0,(i-1)*(n_subdivisions+1)+j);
			      if (i==n_subdivisions) 
				nrml[i*(n_subdivisions+1)+j](0)+=patch->data(0,i*(n_subdivisions+1)+j);
			      else 
				nrml[i*(n_subdivisions+1)+j](0)+=patch->data(0,(i+1)*(n_subdivisions+1)+j);
			      nrml[i*(n_subdivisions+1)+j](0)/=(1./n_subdivisions);
			      if (i!=n_subdivisions&&i!=0) nrml[i*(n_subdivisions+1)+j](0)/=2;
			      if (j==0)
				nrml[i*(n_subdivisions+1)+j](2)=-patch->data(0,i*(n_subdivisions+1)+j);
			      else
				nrml[i*(n_subdivisions+1)+j](2)=-patch->data(0,i*(n_subdivisions+1)+j-1);
			      if (j==n_subdivisions)
				nrml[i*(n_subdivisions+1)+j](2)+=patch->data(0,i*(n_subdivisions+1)+j);
			      else
				nrml[i*(n_subdivisions+1)+j](2)+=patch->data(0,i*(n_subdivisions+1)+j+1);
			      nrml[i*(n_subdivisions+1)+j](2)/=-(1./n_subdivisions);
			      if (j!=n_subdivisions&&j!=0) nrml[i*(n_subdivisions+1)+j](2)/=2;
			      nrml[i*(n_subdivisions+1)+j](1)=1.;

			                                        // normalize Vektor
			      double norm=sqrt(
					      pow(nrml[i*(n_subdivisions+1)+j](0),2.)+
					      pow(nrml[i*(n_subdivisions+1)+j](1),2.)+1.);
			      
			      for (unsigned int k=0;k<3;++k)  nrml[i*(n_subdivisions+1)+j](k)/=norm;
			    }
			}
		                                             // writing smooth_triangles
				
							     // down/right triangle
		      out << "smooth_triangle {" << endl << "\t<" 
			  << ver[dl](0) << ","   
			  << patch->data(0,dl) << ","
			  << ver[dl](1) << ">, <"
			  << nrml[dl] << ">," << endl
			  << " \t<" 
			  << ver[dl+n_subdivisions+1](0) << "," 
			  << patch->data(0,dl+n_subdivisions+1)  << ","
			  << ver[dl+n_subdivisions+1](1) << ">, <"
			  << nrml[dl+n_subdivisions+1] << ">," << endl 
			  << "\t<" 
			  << ver[dl+n_subdivisions+2](0) << "," 
			  << patch->data(0,dl+n_subdivisions+2)  << ","
			  << ver[dl+n_subdivisions+2](1) << ">, <"
			  << nrml[dl+n_subdivisions+2] << ">}" << endl; 
		      
						     // upper/left triangle
		      out << "smooth_triangle {" << endl << "\t<" 
			  << ver[dl](0) << "," 
			  << patch->data(0,dl) << ","
			  << ver[dl](1) << ">, <"
			  << nrml[dl] << ">," << endl 
			  << "\t<" 
			  << ver[dl+n_subdivisions+2](0) << "," 
			  << patch->data(0,dl+n_subdivisions+2)  << ","
			  << ver[dl+n_subdivisions+2](1) << ">, <"
			  << nrml[dl+n_subdivisions+2] << ">," << endl 
			  << "\t<" 
			  << ver[dl+1](0) << "," 
			  << patch->data(0,dl+1)  << ","
			  << ver[dl+1](1) << ">, <"
			  << nrml[dl+1] << ">}" << endl;
		    }
		  else
		    {		
	                                    // writing standard triangles
                                            // down/right triangle
		      out << "triangle {" << endl << "\t<" 
			  << ver[dl](0) << "," 
			  << patch->data(0,dl) << ","
			  << ver[dl](1) << ">," << endl
			  << "\t<" 
			  << ver[dl+n_subdivisions+1](0) << "," 
			  << patch->data(0,dl+n_subdivisions+1)  << ","
			  << ver[dl+n_subdivisions+1](1) << ">," << endl 
			  << "\t<" 
			  << ver[dl+n_subdivisions+2](0) << "," 
			  << patch->data(0,dl+n_subdivisions+2)  << ","
			  << ver[dl+n_subdivisions+2](1) << ">}" << endl; 
			    
						     // upper/left triangle
		      out << "triangle {" << endl << "\t<" 
			  << ver[dl](0) << "," 
			  << patch->data(0,dl) << ","
			  << ver[dl](1) << ">," << endl 
			  << "\t<"
			  << ver[dl+n_subdivisions+2](0) << "," 
			  << patch->data(0,dl+n_subdivisions+2)  << ","
			  << ver[dl+n_subdivisions+2](1) << ">," << endl 
			  << "\t<" 
			  << ver[dl+1](0) << "," 
			  << patch->data(0,dl+1)  << ","
			  << ver[dl+1](1) << ">}" << endl;
		    };
		};
	    };
	}
      else
	{                                    // writing bicubic_patch
	  Assert (n_subdivisions==3, ExcUnexpectedNumberOfSubdivisions(n_subdivisions,3));
	  out << endl
	      << "bicubic_patch {" << endl
	      << "  type 0" << endl
	      << "  flatness 0" << endl
	      << "  u_steps 0" << endl
	      << "  v_steps 0" << endl;
	  for (int i=0;i<16;++i)
	    {
	      out << "\t<" << ver[i](0) << "," << patch->data(0,i) << "," << ver[i](1) << ">";
	      if (i!=15) out << ",";
	      out << endl;
	    };
	  out << "  texture {Tex}" <<  endl
	      << "}" << endl;
	};
    };
  
  if (!flags.bicubic_patch) 
    {                                   // the end of the mesh
      out << "  texture {Tex}" << endl
	  << "}" << endl
	  << endl;
    }
  
  AssertThrow (out, ExcIO());
};


template <int dim>
void DataOutBase::write_eps (const vector<Patch<dim> > &patches,
			     const vector<string>      &/*data_names*/,
			     const EpsFlags            &flags,
			     ostream                   &out) 
{
  Assert (out, ExcIO());
  
  Assert (patches.size() > 0, ExcNoPatches());
  
  switch (dim) 
    {
      case 2:
      {
					 // set up an array of cells to be
					 // written later. this array holds the
					 // cells of all the patches as
					 // projected to the plane perpendicular
					 // to the line of sight.
					 //
					 // note that they are kept sorted by
					 // the set, where we chose the value
					 // of the center point of the cell
					 // along the line of sight as value
					 // for sorting
	multiset<EpsCell2d> cells;

					 // two variables in which we
					 // will store the minimum and
					 // maximum values of the field
					 // to be used for colorization
					 //
					 // preset them by 0 to calm down the
					 // compiler; they are initialized later
	double min_color_value=0, max_color_value=0;
	
					 // compute the cells for output and
					 // enter them into the set above
					 // note that since dim==2, we
					 // have exactly four vertices per
					 // patch and per cell
	for (vector<Patch<dim> >::const_iterator patch=patches.begin();
	     patch!=patches.end(); ++patch)
	  {
	    const unsigned int n_subdivisions = patch->n_subdivisions;
	    for (unsigned int i=0; i<n_subdivisions; ++i)
	      for (unsigned int j=0; j<n_subdivisions; ++j)
		{
		  const double x_frac = i * 1./n_subdivisions,
			       y_frac = j * 1./n_subdivisions,
					
			      x_frac1 = (i+1) * 1./n_subdivisions,
			      y_frac1 = (j+1) * 1./n_subdivisions;
		  
		  const Point<dim> points[4]
		    = { (((patch->vertices[1] * x_frac) +
			  (patch->vertices[0] * (1-x_frac))) * (1-y_frac) +
			 ((patch->vertices[2] * x_frac) +
			  (patch->vertices[3] * (1-x_frac))) * y_frac),

			(((patch->vertices[1] * x_frac1) +
			  (patch->vertices[0] * (1-x_frac1))) * (1-y_frac) +
			 ((patch->vertices[2] * x_frac1) +
			  (patch->vertices[3] * (1-x_frac1))) * y_frac),

			(((patch->vertices[1] * x_frac1) +
			  (patch->vertices[0] * (1-x_frac1))) * (1-y_frac1) +
			 ((patch->vertices[2] * x_frac1) +
			  (patch->vertices[3] * (1-x_frac1))) * y_frac1),

			(((patch->vertices[1] * x_frac) +
			  (patch->vertices[0] * (1-x_frac))) * (1-y_frac1) +
			 ((patch->vertices[2] * x_frac) +
			  (patch->vertices[3] * (1-x_frac))) * y_frac1) 
		    };

		  Assert ((flags.height_vector < patch->data.m()) ||
			  patch->data.m() == 0,
			  ExcInvalidVectorNumber (flags.height_vector,
						  patch->data.m()));
		  const double heights[4]
		    = { patch->data.m() != 0 ?
			patch->data(flags.height_vector,i*(n_subdivisions+1) + j)       * flags.z_scaling : 0,
			
			patch->data.m() != 0 ?
			patch->data(flags.height_vector,(i+1)*(n_subdivisions+1) + j)   * flags.z_scaling : 0,
			
			patch->data.m() != 0 ?
			patch->data(flags.height_vector,(i+1)*(n_subdivisions+1) + j+1) * flags.z_scaling : 0,
			
			patch->data.m() != 0 ?
			patch->data(flags.height_vector,i*(n_subdivisions+1) + j+1)     * flags.z_scaling : 0};


// now compute the projection of
						   // the bilinear cell given by the
						   // four vertices and their heights
						   // and write them to a proper
						   // cell object. note that we only
						   // need the first two components
						   // of the projected position for
						   // output, but we need the value
						   // along the line of sight for
						   // sorting the cells for back-to-
						   // front-output
						   //
						   // this computation was first written
						   // by Stefan Nauber. please no-one
						   // ask me why it works that way (or
						   // may be not), especially not about
						   // the angles and the sign of
						   // the height field, I don't know
						   // it.
		  EpsCell2d eps_cell;
		  const double pi = 3.1415926536;
		  const double cx = -cos(pi-flags.azimut_angle * 2*pi / 360.),
			       cz = -cos(flags.turn_angle * 2*pi / 360.),
			       sx = sin(pi-flags.azimut_angle * 2*pi / 360.),
			       sz = sin(flags.turn_angle * 2*pi / 360.);
		  for (unsigned int vertex=0; vertex<4; ++vertex)
		    {
		      const double x = points[vertex](0),
				   y = points[vertex](1),
				   z = -heights[vertex];
		      
		      eps_cell.vertices[vertex](0) = -   cz*x+   sz*y;
		      eps_cell.vertices[vertex](1) = -cx*sz*x-cx*cz*y-sx*z;

						       //      ( 1 0    0 )
						       // Dx = ( 0 cx -sx ) 
						       //      ( 0 sx  cx )

						       //      ( cy 0 sy )
						       // Dy = (  0 1  0 )
						       //      (-sy 0 cy )

						       //      ( cz -sz 0 )
						       // Dz = ( sz  cz 0 )
						       //      (  0   0 1 )

//       ( cz -sz 0 )( 1 0    0 )(x)   ( cz*x-sz*(cx*y-sx*z)+0*(sx*y+cx*z) )
// Dxz = ( sz  cz 0 )( 0 cx -sx )(y) = ( sz*x+cz*(cx*y-sx*z)+0*(sx*y+cx*z) )
// 	 (  0   0 1 )( 0 sx  cx )(z)   (  0*x+	*(cx*y-sx*z)+1*(sx*y+cx*z) )
		    };

						   // compute coordinates of
						   // center of cell
		  const Point<dim> center_point
		    = (points[0] + points[1] + points[2] + points[3]) / 4;
		  const double center_height
		    = -(heights[0] + heights[1] + heights[2] + heights[3]) / 4;

						   // compute the depth into
						   // the picture
		  eps_cell.depth = -sx*sz*center_point(0)
				   -sx*cz*center_point(1)
				   +cx*center_height;

		  if (flags.draw_cells && flags.shade_cells)
		    {
		      Assert ((flags.color_vector < patch->data.m()) ||
			      patch->data.m() == 0,
			      ExcInvalidVectorNumber (flags.color_vector,
						      patch->data.m()));
		      const double color_values[4]
			= { patch->data.m() != 0 ?
			    patch->data(flags.color_vector,i*(n_subdivisions+1) + j)       : 1,
			
			    patch->data.m() != 0 ?
			    patch->data(flags.color_vector,(i+1)*(n_subdivisions+1) + j)   : 1,
			    
			    patch->data.m() != 0 ?
			    patch->data(flags.color_vector,(i+1)*(n_subdivisions+1) + j+1) : 1,
			    
			    patch->data.m() != 0 ?
			    patch->data(flags.color_vector,i*(n_subdivisions+1) + j+1)     : 1};

						       // set color value to average of the value
						       // at the vertices
		      eps_cell.color_value = (color_values[0] +
					      color_values[1] +
					      color_values[2] +
					      color_values[3]) / 4;

						       // update bounds of color
						       // field
		      if (patch == patches.begin())
			min_color_value = max_color_value = eps_cell.color_value;
		      else
			{
			  min_color_value = (min_color_value < eps_cell.color_value ?
					     min_color_value : eps_cell.color_value);
			  max_color_value = (max_color_value > eps_cell.color_value ?
					     max_color_value : eps_cell.color_value);
			};
		    };
		  
						   // finally add this cell
		  cells.insert (eps_cell);
		};
	  };

					 // find out minimum and maximum x and
					 // y coordinates to compute offsets
					 // and scaling factors
	double x_min = cells.begin()->vertices[0](0);
	double x_max = x_min;
	double y_min = cells.begin()->vertices[0](1);
	double y_max = y_min;
	
	for (multiset<EpsCell2d>::const_iterator cell=cells.begin();
	     cell!=cells.end(); ++cell)
	  for (unsigned int vertex=0; vertex<4; ++vertex)
	    {
	      x_min = min (x_min, cell->vertices[vertex](0));
	      x_max = max (x_max, cell->vertices[vertex](0));
	      y_min = min (y_min, cell->vertices[vertex](1));
	      y_max = max (y_max, cell->vertices[vertex](1));
	    };
	
					 // scale in x-direction such that
					 // in the output 0 <= x <= 300.
					 // don't scale in y-direction to
					 // preserve the shape of the
					 // triangulation
	const double scale = (flags.size /
			      (flags.size_type==EpsFlags::width ?
			       x_max - x_min :
			       y_min - y_max));
	
	const Point<2> offset(x_min, y_min);


// now write preamble
	if (true) 
	  {
					     // block this to have local
					     // variables destroyed after
					     // use
	    time_t  time1= time (0);
	    tm     *time = localtime(&time1); 
	    out << "%!PS-Adobe-2.0 EPSF-1.2" << endl
		<< "%%Title: deal.II Output" << endl
		<< "%%Creator: the deal.II library" << endl
		<< "%%Creation Date: " 
		<< time->tm_year+1900 << "/"
		<< time->tm_mon+1 << "/"
		<< time->tm_mday << " - "
		<< time->tm_hour << ":"
		<< setw(2) << time->tm_min << ":"
		<< setw(2) << time->tm_sec << endl
		<< "%%BoundingBox: "
					       // lower left corner
		<< "0 0 "
					       // upper right corner
		<< static_cast<unsigned int>( (x_max-x_min) * scale )
		<< ' '
		<< static_cast<unsigned int>( (y_max-y_min) * scale )
		<< endl;
	    
					     // define some abbreviations to keep
					     // the output small:
					     // m=move turtle to
					     // l=define a line
					     // s=set rgb color
					     // sg=set gray value
					     // lx=close the line and plot the line
					     // lf=close the line and fill the interior
	    out << "/m {moveto} bind def"      << endl
		<< "/l {lineto} bind def"      << endl
		<< "/s {setrgbcolor} bind def" << endl
		<< "/sg {setgray} bind def"    << endl
		<< "/lx {lineto closepath stroke} bind def" << endl
		<< "/lf {lineto closepath fill} bind def"   << endl;
	    
	    out << "%%EndProlog" << endl
		<< endl;
					     // set fine lines
	    out << flags.line_width << " setlinewidth" << endl;
					     // allow only five digits
					     // for output (instead of the
					     // default six); this should suffice
					     // even for fine grids, but reduces
					     // the file size significantly
	    out << setprecision (5);
	  };

					 // now we've got all the information
					 // we need. write the cells.
					 // note: due to the ordering, we
					 // traverse the list of cells
					 // back-to-front
	for (multiset<EpsCell2d>::const_iterator cell=cells.begin();
	     cell!=cells.end(); ++cell)
	  {
	    if (flags.draw_cells) 
	      {
		if (flags.shade_cells)
		  {
		    const EpsFlags::RgbValues rgb_values
		      = (*flags.color_function) (cell->color_value,
						 min_color_value,
						 max_color_value);
		    
		    out << rgb_values.red   << ' '
			<< rgb_values.green << ' '
			<< rgb_values.blue  << " s ";
		  }
		else
		  out << "1 1 1 s ";

		out << (cell->vertices[0]-offset) * scale << " m "
		    << (cell->vertices[1]-offset) * scale << " l "
		    << (cell->vertices[2]-offset) * scale << " l "
		    << (cell->vertices[3]-offset) * scale << " lf"
		    << endl;
	      };
	    
	    if (flags.draw_mesh)
	      out << "0 sg "      // draw lines in black
		  << (cell->vertices[0]-offset) * scale << " m "
		  << (cell->vertices[1]-offset) * scale << " l "
		  << (cell->vertices[2]-offset) * scale << " l "
		  << (cell->vertices[3]-offset) * scale << " lx"
		  << endl;
	  };
	out << "showpage" << endl;
	
	break;
      };
       
      default:
	    Assert (false, ExcNotImplemented());
    };
};


template <int dim>
void DataOutBase::write_gmv (const vector<Patch<dim> > &patches,
			     const vector<string>      &data_names,
			     const GmvFlags            &/*flags*/,
			     ostream                   &out) 
{
  AssertThrow (out, ExcIO());

  Assert (patches.size() > 0, ExcNoPatches());
 
  const unsigned int n_data_sets = data_names.size();
  
				   ///////////////////////
				   // preamble
  out << "gmvinput ascii"
      << endl
      << endl;

				   // first count the number of cells
				   // and cells for later use
  unsigned int n_cells = 0,
	       n_nodes = 0;
  for (vector<Patch<dim> >::const_iterator patch=patches.begin();
       patch!=patches.end(); ++patch)
    switch (dim)
      {
	case 1:
	      n_cells += patch->n_subdivisions;
	      n_nodes += patch->n_subdivisions+1;
	      break;
	case 2:
	      n_cells += patch->n_subdivisions *
			 patch->n_subdivisions;
	      n_nodes += (patch->n_subdivisions+1) *
			 (patch->n_subdivisions+1);
	      break;
	case 3:
	      n_cells += patch->n_subdivisions *
			 patch->n_subdivisions *
			 patch->n_subdivisions;
	      n_nodes += (patch->n_subdivisions+1) *
			 (patch->n_subdivisions+1) *
			 (patch->n_subdivisions+1);
	      break;
	default:
	      Assert (false, ExcNotImplemented());
      };


///////////////////////////////
				   // first make up a list of used
				   // vertices along with their
				   // coordinates
				   //
				   // note that we have to print
				   // d=1..3 dimensions
  out << "nodes " << n_nodes << endl;
  for (unsigned int d=1; d<=3; ++d)
    {
      for (vector<Patch<dim> >::const_iterator patch=patches.begin();
	   patch!=patches.end(); ++patch)
	{
	  const unsigned int n_subdivisions = patch->n_subdivisions;
	  
					   // if we have nonzero values for
					   // this coordinate
	  if (d<=dim)
	    {
	      switch (dim)
		{
		  case 1:
		  {
		    for (unsigned int i=0; i<n_subdivisions+1; ++i)
		      out << ((patch->vertices[1](0) * i / n_subdivisions) +
			      (patch->vertices[0](0) * (n_subdivisions-i) / n_subdivisions))
			  << ' ';
		    break;
		  };
		   
		  case 2:
		  {
		    for (unsigned int i=0; i<n_subdivisions+1; ++i)
		      for (unsigned int j=0; j<n_subdivisions+1; ++j)
			{
			  const double x_frac = i * 1./n_subdivisions,
				       y_frac = j * 1./n_subdivisions;
			  
							   // compute coordinates for
							   // this patch point
			  out << (((patch->vertices[1](d-1) * x_frac) +
				   (patch->vertices[0](d-1) * (1-x_frac))) * (1-y_frac) +
				  ((patch->vertices[2](d-1) * x_frac) +
				   (patch->vertices[3](d-1) * (1-x_frac))) * y_frac)
			      << ' ';
			};
		    break;
		  };
		   
		  case 3:
		  {
		    for (unsigned int i=0; i<n_subdivisions+1; ++i)
		      for (unsigned int j=0; j<n_subdivisions+1; ++j)
			for (unsigned int k=0; k<n_subdivisions+1; ++k)
			  {
							     // note the broken
							     // design of hexahedra
							     // in deal.II, where
							     // first the z-component
							     // is counted up, before
							     // increasing the y-
							     // coordinate
			    const double x_frac = i * 1./n_subdivisions,
					 y_frac = k * 1./n_subdivisions,
					 z_frac = j * 1./n_subdivisions;
			    
							     // compute coordinates for
							     // this patch point
			    out << ((((patch->vertices[1](d-1) * x_frac) +
				      (patch->vertices[0](d-1) * (1-x_frac))) * (1-y_frac) +
				     ((patch->vertices[2](d-1) * x_frac) +
				      (patch->vertices[3](d-1) * (1-x_frac))) * y_frac)   * (1-z_frac) +
				    (((patch->vertices[5](d-1) * x_frac) +
				      (patch->vertices[4](d-1) * (1-x_frac))) * (1-y_frac) +
				     ((patch->vertices[6](d-1) * x_frac) +
				      (patch->vertices[7](d-1) * (1-x_frac))) * y_frac)   * z_frac)
				<< ' ';
			  };
	      
		    break;
		  };
		   
		  default:
			Assert (false, ExcNotImplemented());
		};
	    }
	  else
					     // d>dim. write zeros instead
	    {
	      const unsigned int n_points
		= static_cast<unsigned int>(pow (n_subdivisions+1, dim));
	      for (unsigned int i=0; i<n_points; ++i)
		out << "0 ";
	    };
	};
      out << endl;
    };

  out << endl;

				   /////////////////////////////////
				   // now for the cells. note that
				   // vertices are counted from 1 onwards
  if (true)
    {
      out << "cells " << n_cells << endl;


unsigned int first_vertex_of_patch = 0;
      
      for (vector<Patch<dim> >::const_iterator patch=patches.begin();
	   patch!=patches.end(); ++patch)
	{
	  const unsigned int n_subdivisions = patch->n_subdivisions;

					   // write out the cells making
					   // up this patch
	  switch (dim)
	    {
	      case 1:
	      {
		for (unsigned int i=0; i<n_subdivisions; ++i)
		  out << "line 2\n  "
		      << first_vertex_of_patch+i+1 << ' '
		      << first_vertex_of_patch+i+1+1 << endl;
		break;
	      };
	       
	      case 2:
	      {
		for (unsigned int i=0; i<n_subdivisions; ++i)
		  for (unsigned int j=0; j<n_subdivisions; ++j)
		    out << "quad 4\n  "
			<< first_vertex_of_patch+i*(n_subdivisions+1)+j+1 << ' '
			<< first_vertex_of_patch+(i+1)*(n_subdivisions+1)+j+1 << ' '
			<< first_vertex_of_patch+(i+1)*(n_subdivisions+1)+j+1+1 << ' '
			<< first_vertex_of_patch+i*(n_subdivisions+1)+j+1+1
			<< endl;
		break;
	      };
	       
	      case 3:
	      {
		for (unsigned int i=0; i<n_subdivisions; ++i)
		  for (unsigned int j=0; j<n_subdivisions; ++j)
		    for (unsigned int k=0; k<n_subdivisions; ++k)
		      {
			out << "hex 8\n   "
							   // note: vertex indices start with 1!
			    << first_vertex_of_patch+(i*(n_subdivisions+1)+j      )*(n_subdivisions+1)+k  +1 << ' '
			    << first_vertex_of_patch+((i+1)*(n_subdivisions+1)+j  )*(n_subdivisions+1)+k  +1 << ' '
			    << first_vertex_of_patch+((i+1)*(n_subdivisions+1)+j+1)*(n_subdivisions+1)+k  +1 << ' '
			    << first_vertex_of_patch+(i*(n_subdivisions+1)+j+1    )*(n_subdivisions+1)+k  +1 << ' '
			    << first_vertex_of_patch+(i*(n_subdivisions+1)+j      )*(n_subdivisions+1)+k+1+1 << ' '
			    << first_vertex_of_patch+((i+1)*(n_subdivisions+1)+j  )*(n_subdivisions+1)+k+1+1 << ' '
			    << first_vertex_of_patch+((i+1)*(n_subdivisions+1)+j+1)*(n_subdivisions+1)+k+1+1 << ' '
			    << first_vertex_of_patch+(i*(n_subdivisions+1)+j+1    )*(n_subdivisions+1)+k+1+1 << ' '
			    << endl;
		      };
		break;
	      };

	      default:
		    Assert (false, ExcNotImplemented());
	    };


// finally update the number
					   // of the first vertex of this patch
	  switch (dim)
	    {
	      case 1:
		    first_vertex_of_patch += n_subdivisions+1;
		    break;
	      case 2:
		    first_vertex_of_patch += (n_subdivisions+1) *
					     (n_subdivisions+1);
		    break;
	      case 3:
		    first_vertex_of_patch += (n_subdivisions+1) *
					     (n_subdivisions+1) *
					     (n_subdivisions+1);
		    break;
	      default:
		    Assert (false, ExcNotImplemented());
	    };
	};
      out << endl;
    };

				   ///////////////////////////////////////
				   // data output.
  out << "variable" << endl;

				   // since here as with the vertex
				   // coordinates the order is a bit
				   // unpleasant (first all data of
				   // variable 1, then variable 2, etc)
				   // we have to copy them a bit around
				   //
				   // note that we copy vectors when
				   // looping over the patches since we
				   // have to write them one variable
				   // at a time and don't want to use
				   // more than one loop
  if (true)
    {
      vector<vector<double> > data_vectors (n_data_sets,
					    vector<double> (n_nodes));
				       // loop over all patches
      unsigned int next_value = 0;
      for (typename vector<Patch<dim> >::const_iterator patch=patches.begin();
	   patch != patches.end(); ++patch)
	{
	  const unsigned int n_subdivisions = patch->n_subdivisions;
	  
	  Assert (patch->data.m() == n_data_sets,
		  ExcUnexpectedNumberOfDatasets (patch->data.m(), n_data_sets));
	  Assert (patch->data.n() == (dim==1 ?
				      n_subdivisions+1 :
				      (dim==2 ?
				       (n_subdivisions+1)*(n_subdivisions+1) :
				       (dim==3 ?
					(n_subdivisions+1)*(n_subdivisions+1)*(n_subdivisions+1) :
					0))),
		  ExcInvalidDatasetSize (patch->data.n(), n_subdivisions+1));
	  
	  switch (dim)
	    {
	      case 1:
	      {      
		for (unsigned int i=0; i<n_subdivisions+1; ++i, ++next_value) 
		  for (unsigned int data_set=0; data_set<n_data_sets; ++data_set)
		    data_vectors[data_set][next_value] = patch->data(data_set,i);
		
		break;
	      };
		     
	      case 2:
	      {
		for (unsigned int i=0; i<n_subdivisions+1; ++i)
		  for (unsigned int j=0; j<n_subdivisions+1; ++j)
		    {
		      for (unsigned int data_set=0; data_set<n_data_sets; ++data_set)
			data_vectors[data_set][next_value]
			  = patch->data(data_set,i*(n_subdivisions+1) + j);
		      ++next_value;
		    };
		
		break;
	      };
	       
	      case 3:
	      {
		for (unsigned int i=0; i<n_subdivisions+1; ++i)
		  for (unsigned int j=0; j<n_subdivisions+1; ++j)
		    for (unsigned int k=0; k<n_subdivisions+1; ++k)
		      {
			for (unsigned int data_set=0; data_set<n_data_sets; ++data_set)
			  data_vectors[data_set][next_value]
			    = patch->data(data_set,
					  (i*(n_subdivisions+1)+j)*(n_subdivisions+1)+k);
			++next_value;
		      };

		break;
	      };
	       
	      default:
		    Assert (false, ExcNotImplemented());
	    };
	};

				       // now write the data vectors to #out#
				       // the '1' means: node data (as opposed
				       // to cell data, which we do not
				       // support explicitely here)
      for (unsigned int data_set=0; data_set<n_data_sets; ++data_set)
	{
	  out << data_names[data_set] << " 1" << endl;
	  copy(data_vectors[data_set].begin(),
	       data_vectors[data_set].end(),
	       ostream_iterator<double>(out, " "));
	  out << endl
	      << endl;
	};
    };


// end of variable section
  out << "endvars" << endl;
  
				   // end of output
  out << "endgmv"
      << endl;
  
				   // assert the stream is still ok
  AssertThrow (out, ExcIO());
};


/* --------------------------- class DataOutInterface ---------------------- */


template <int dim>
void DataOutInterface<dim>::write_ucd (ostream &out) const 
{
  DataOutBase::write_ucd (get_patches(), get_dataset_names(),
			  ucd_flags, out);
};


template <int dim>
void DataOutInterface<dim>::write_gnuplot (ostream &out) const 
{
  DataOutBase::write_gnuplot (get_patches(), get_dataset_names(),
			      gnuplot_flags, out);
};


template <int dim>
void DataOutInterface<dim>::write_povray (ostream &out) const 
{
  DataOutBase::write_povray (get_patches(), get_dataset_names(),
			     povray_flags, out);
};


template <int dim>
void DataOutInterface<dim>::write_eps (ostream &out) const 
{
  DataOutBase::write_eps (get_patches(), get_dataset_names(),
			  eps_flags, out);
};


template <int dim>
void DataOutInterface<dim>::write_gmv (ostream &out) const 
{
  DataOutBase::write_gmv (get_patches(), get_dataset_names(),
			  gmv_flags, out);
};


template <int dim>
void DataOutInterface<dim>::write (ostream &out,
				   OutputFormat output_format) const
{
  if (output_format == default_format)
    output_format = default_fmt;
  
  switch (output_format) 
    {
      case ucd:
	    write_ucd (out);
	    break;
	    
      case gnuplot:
	    write_gnuplot (out);
	    break;
	    
      case povray:
	    write_povray (out);
	    break;
	    
      case eps:
	    write_eps(out);
	    break;
	    
      case gmv:
	    write_gmv (out);
	    break;
	    
      default:
	    Assert (false, ExcNotImplemented());
    };
};


template <int dim>
void DataOutInterface<dim>::set_default_format(OutputFormat fmt)
{
  Assert(fmt != default_format, ExcNotImplemented());
  default_fmt = fmt;
}


template <int dim>
void DataOutInterface<dim>::set_flags (const UcdFlags &flags) 
{
  ucd_flags = flags;
};


template <int dim>
void DataOutInterface<dim>::set_flags (const GnuplotFlags &flags) 
{
  gnuplot_flags = flags;
};


template <int dim>
void DataOutInterface<dim>::set_flags (const PovrayFlags &flags) 
{
  povray_flags = flags;
};


template <int dim>
void DataOutInterface<dim>::set_flags (const EpsFlags &flags) 
{
  eps_flags = flags;
};


template <int dim>
void DataOutInterface<dim>::set_flags (const GmvFlags &flags) 
{
  gmv_flags = flags;
};


template <int dim>
string DataOutInterface<dim>::default_suffix (OutputFormat output_format) 
{
  if (output_format == default_format)
    output_format = default_fmt;
  
  switch (output_format) 
    {
      case ucd:
	    return ".inp";
	    
      case gnuplot: 
	    return ".gnuplot";
	    
      case povray: 
	    return ".pov";
	    
      case eps: 
	    return ".eps";

      case gmv:
	    return ".gmv";
	    
      default: 
	    Assert (false, ExcNotImplemented()); 
	    return "";
    };
};


template <int dim>
DataOutInterface<dim>::OutputFormat
DataOutInterface<dim>::parse_output_format (const string &format_name)
{
  if (format_name == "ucd")
    return ucd;

  if (format_name == "gnuplot")
    return gnuplot;

  if (format_name == "povray")
    return povray;

  if (format_name == "eps")
    return eps;

  if (format_name == "gmv")
    return gmv;
  
  AssertThrow (false, ExcInvalidState ());

				   // return something invalid
  return OutputFormat(-1);
};


template <int dim>
string DataOutInterface<dim>::get_output_format_names ()
{
  return "ucd|gnuplot|povray|eps|gmv";
}


template <int dim>
void DataOutInterface<dim>::declare_parameters (ParameterHandler &prm) 
{
  prm.declare_entry ("Output format", "gnuplot",
		     Patterns::Selection (get_output_format_names ()));

  prm.enter_subsection ("UCD output parameters");
  UcdFlags::declare_parameters (prm);
  prm.leave_subsection ();
  
  prm.enter_subsection ("Gnuplot output parameters");
  GnuplotFlags::declare_parameters (prm);
  prm.leave_subsection ();

  prm.enter_subsection ("Povray output parameters");
  PovrayFlags::declare_parameters (prm);
  prm.leave_subsection ();

  prm.enter_subsection ("Eps output parameters");
  EpsFlags::declare_parameters (prm);
  prm.leave_subsection ();

  prm.enter_subsection ("Gmv output parameters");
  GmvFlags::declare_parameters (prm);
  prm.leave_subsection ();
}


template <int dim>
void DataOutInterface<dim>::parse_parameters (ParameterHandler &prm) 
{
  const string& output_name = prm.get ("Output format");
  default_fmt = parse_output_format (output_name);

  prm.enter_subsection ("UCD output parameters");
  ucd_flags.parse_parameters (prm);
  prm.leave_subsection ();
  
  prm.enter_subsection ("Gnuplot output parameters");
  gnuplot_flags.parse_parameters (prm);
  prm.leave_subsection ();

  prm.enter_subsection ("Povray output parameters");
  povray_flags.parse_parameters (prm);
  prm.leave_subsection ();

  prm.enter_subsection ("Eps output parameters");
  eps_flags.parse_parameters (prm);
  prm.leave_subsection ();

  prm.enter_subsection ("Gmv output parameters");
  gmv_flags.parse_parameters (prm);
  prm.leave_subsection ();
}


// explicit instantiations. functions in DataOutBase are instantiated by
// the respective functions in DataOut_Interface
template class DataOutInterface<data_out_dimension>;

