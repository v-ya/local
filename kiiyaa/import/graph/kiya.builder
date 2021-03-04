: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "fuwafuwa"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[$ "kiya"]          "name"        @string => "import.graph"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "libgraph.so"]
	# graph.h
	"graph_alloc"                          @null
	"graph_init_instance"                  @null
	"graph_instance_enable_debug_message"  @null
	# allocator.h
	# support.h
	"graph_layer_support_get_instance"      @null
	"graph_layer_support_get_dev"           @null
	"graph_extension_support_get_instance"  @null
	"graph_extension_support_get_dev"       @null
	"graph_layer_support_test"              @null
	"graph_layer_support_all"               @null
	"graph_layer_support_dump"              @null
	"graph_extension_support_test"          @null
	"graph_extension_support_all"           @null
	"graph_extension_support_dump"          @null
	# device.h
	"graph_instance_devices_get"                         @null
	"graph_devices_number"                               @null
	"graph_devices_index"                                @null
	"graph_device_queues_get"                            @null
	"graph_device_queues_number"                         @null
	"graph_device_queues_index"                          @null
	"graph_dev_param_alloc"                              @null
	"graph_dev_param_set_queue"                          @null
	"graph_dev_param_set_layer"                          @null
	"graph_dev_param_set_extension"                      @null
	"graph_dev_param_feature_enable"                     @null
	"graph_dev_param_feature_disable"                    @null
	"graph_dev_param_feature_enable_all"                 @null
	"graph_dev_alloc"                                    @null
	"graph_dev_queue"                                    @null
	"graph_dev_wait_idle"                                @null
	"graph_device_queue_flags"                           @null
	"graph_device_queue_count"                           @null
	"graph_device_queue_timestamp_valid_bits"            @null
	"graph_device_queue_min_image_transfer_granularity"  @null
	"graph_device_features_test"                         @null
	"graph_device_queue_surface_support"                 @null
	"graph_device_properties_limits_dump"                @null
	"graph_device_properties_sparse_dump"                @null
	"graph_device_properties_dump"                       @null
	"graph_device_features_dump"                         @null
	"graph_device_dump"                                  @null
	"graph_device_queue_dump"                            @null
	# surface.h
	"graph_surface_xcb_create_window"       @null
	"graph_surface_attr_get"                @null
	"graph_surface_do_event"                @null
	"graph_surface_do_events"               @null
	"graph_surface_set_event_data"          @null
	"graph_surface_register_event_close"    @null
	"graph_surface_register_event_expose"   @null
	"graph_surface_register_event_key"      @null
	"graph_surface_register_event_button"   @null
	"graph_surface_register_event_pointer"  @null
	"graph_surface_register_event_area"     @null
	"graph_surface_register_event_focus"    @null
	"graph_surface_register_event_config"   @null
	"graph_surface_set_event"               @null
	"graph_surface_get_geometry"            @null
	"graph_surface_resize"                  @null
	"graph_swapchain_param_alloc"           @null
	"graph_swapchain_alloc"                 @null
	"graph_swapchain_rebulid"               @null
	"graph_swapchain_image_number"          @null
	"graph_swapchain_format"                @null
	"graph_swapchain_info"                  @null
	"graph_swapchain_acquire"               @null
	"graph_surface_attr_dump"               @null
	# image.h
	"graph_image_alloc_1d"                 @null
	"graph_image_alloc_2d"                 @null
	"graph_image_alloc_3d"                 @null
	"graph_image_view_param_alloc"         @null
	"graph_image_view_alloc"               @null
	"graph_image_view_alloc_by_swapchain"  @null
	"graph_frame_buffer_alloc"             @null
	"graph_sampler_param_alloc"            @null
	"graph_sampler_param_set_flags"        @null
	"graph_sampler_param_set_filter"       @null
	"graph_sampler_param_set_mipmap"       @null
	"graph_sampler_param_set_address"      @null
	"graph_sampler_param_set_anisotropy"   @null
	"graph_sampler_param_set_compare"      @null
	"graph_sampler_param_set_border"       @null
	"graph_sampler_param_set_coordinates"  @null
	"graph_sampler_alloc"                  @null
	# layout.h
	"graph_descriptor_set_layout_param_alloc"           @null
	"graph_descriptor_set_layout_param_set_binding"     @null
	"graph_descriptor_set_layout_alloc"                 @null
	"graph_pipe_layout_param_alloc"                     @null
	"graph_pipe_layout_param_append_set_layout"         @null
	"graph_pipe_layout_param_append_range"              @null
	"graph_pipe_layout_alloc"                           @null
	"graph_descriptor_pool_alloc"                       @null
	"graph_descriptor_sets_alloc"                       @null
	"graph_descriptor_sets_info_alloc"                  @null
	"graph_descriptor_sets_info_append_write"           @null
	"graph_descriptor_sets_info_set_write_image_info"   @null
	"graph_descriptor_sets_info_set_write_buffer_info"  @null
	"graph_descriptor_sets_info_update"                 @null
	# shader.h
	"graph_shader_alloc"                                    @null
	"graph_vertex_input_description_alloc"                  @null
	"graph_vertex_input_description_set_bind"               @null
	"graph_vertex_input_description_set_attr"               @null
	"graph_viewports_scissors_alloc"                        @null
	"graph_viewports_scissors_append_viewport"              @null
	"graph_viewports_scissors_append_scissor"               @null
	"graph_viewports_scissors_update_viewport"              @null
	"graph_viewports_scissors_update_scissor"               @null
	"graph_pipe_color_blend_alloc"                          @null
	"graph_pipe_color_blend_set_logic"                      @null
	"graph_pipe_color_blend_set_constants"                  @null
	"graph_pipe_color_blend_append_attachment"              @null
	"graph_render_pass_param_alloc"                         @null
	"graph_render_pass_param_set_attachment"                @null
	"graph_render_pass_param_set_subpass"                   @null
	"graph_render_pass_param_set_subpass_color"             @null
	"graph_render_pass_alloc"                               @null
	"graph_pipe_cache_alloc"                                @null
	"graph_pipe_alloc_graphics"                             @null
	"graph_gpipe_param_alloc"                               @null
	"graph_gpipe_param_append_shader"                       @null
	"graph_gpipe_param_set_vertex"                          @null
	"graph_gpipe_param_set_assembly"                        @null
	"graph_gpipe_param_set_viewport"                        @null
	"graph_gpipe_param_set_color_blend"                     @null
	"graph_gpipe_param_set_rasterization_depth_clamp"       @null
	"graph_gpipe_param_set_rasterization_discard"           @null
	"graph_gpipe_param_set_rasterization_polygon"           @null
	"graph_gpipe_param_set_rasterization_cull"              @null
	"graph_gpipe_param_set_rasterization_front_face"        @null
	"graph_gpipe_param_set_rasterization_depth_bias"        @null
	"graph_gpipe_param_set_rasterization_line_width"        @null
	"graph_gpipe_param_set_multisample_sample_shading"      @null
	"graph_gpipe_param_set_multisample_min_sample_shading"  @null
	"graph_gpipe_param_set_multisample_alpha2coverage"      @null
	"graph_gpipe_param_set_multisample_alpha2one"           @null
	"graph_gpipe_param_set_dynamic"                         @null
	"graph_gpipe_param_set_layout"                          @null
	"graph_gpipe_param_set_render"                          @null
	"graph_gpipe_param_ok"                                  @null
	# command.h
	"graph_command_pool_alloc"                           @null
	"graph_command_pool_begin"                           @null
	"graph_command_pool_end"                             @null
	"graph_command_begin_render"                         @null
	"graph_command_bind_pipe"                            @null
	"graph_command_set_viewport"                         @null
	"graph_command_set_scissor"                          @null
	"graph_command_bind_vertex_buffers"                  @null
	"graph_command_bind_index_buffer"                    @null
	"graph_command_bind_desc_sets"                       @null
	"graph_command_draw"                                 @null
	"graph_command_draw_index"                           @null
	"graph_command_end_render"                           @null
	"graph_command_copy_buffer"                          @null
	"graph_command_copy_buffer_to_image"                 @null
	"graph_command_pipe_barrier_param_alloc"             @null
	"graph_command_pipe_barrier_param_set_image"         @null
	"graph_command_pipe_barrier_param_set_image_access"  @null
	"graph_command_pipe_barrier_param_set_image_layout"  @null
	"graph_command_pipe_barrier_param_set_image_queue"   @null
	"graph_command_pipe_barrier"                         @null
	"graph_queue_submit"                                 @null
	"graph_queue_present"                                @null
	"graph_queue_wait_idle"                              @null
	"graph_semaphore_alloc"                              @null
	"graph_semaphore_signal"                             @null
	"graph_semaphore_wait"                               @null
	"graph_fence_alloc"                                  @null
	"graph_fence_reset"                                  @null
	"graph_fence_wait"                                   @null
	# buffer.h
	"graph_memory_heap_alloc"  @null
	"graph_buffer_alloc"       @null
	"graph_buffer_map"         @null
	"graph_buffer_unmap"       @null
	"graph_buffer_flush"       @null
	"graph_buffer_invalidate"  @null

[] "kiya.export" @"kiya.export.all"