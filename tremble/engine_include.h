#pragma once

#include "pch.h"

#include "core/game_manager.h"
#include "core/get.h"
#include "core/input/input_manager.h"
#include "core/input/key.h"
#include "core/input/mouse.h"
#include "core/math/math.h"

#include "core/rendering/renderer.h"
#include "core/rendering/constant_buffers.h"
#include "core/rendering/constants_helper.h"
#include "core/rendering/debug_volume.h"
#include "core/rendering/descriptor_heap.h"
#include "core/rendering/dynamic_descriptor_heap.h"
#include "core/rendering/dynamic_upload_buffer.h"
#include "core/rendering/frame_resource.h"
#include "core/rendering/material.h"
#include "core/rendering/pipeline_state.h"
#include "core/rendering/root_signature.h"
#include "core/rendering/shader.h"
#include "core/rendering/texture.h"
#include "core/rendering/upload_buffer.h"
#include "core/rendering/vertex.h"

#include "core/config/config_manager.h"

#include "core/resources/fbx_loader.h"
#include "core/resources/mesh.h"
#include "core/resources/obj_loader.h"
#include "core/resources/resource_manager.h"
#include "core/resources/scene_loader.h"
#include "core/resources/tiny_obj_loader.h"

#include "core/memory/memory_includes.h"

#include "core/scene_graph/component.h"
#include "core/scene_graph/component_vector.h"
#include "core/scene_graph/component_manager.h"
#include "core/scene_graph/scene_graph.h"

#include "core/utilities/debug.h"
#include "core/utilities/octree.h"
#include "core/utilities/stopwatch.h"
#include "core/utilities/timer.h"
#include "core/utilities/types.h"
#include "core/utilities/utilities.h"

#include "core/win32/wic_loader.h"
#include "core/win32/window.h"

#include "components/rendering/camera.h"
#include "components/rendering/renderable.h"
#include "components/rendering/skinned_renderable.h"
#include "components/rendering/light.h"
#include "components/rendering/image_component.h"
#include "components/rendering/button_component.h"
#include "components/rendering/text_component.h"
#include "components/rendering/particle_system.h"

#include "core/physics/physics_manager.h"
#include "core/physics/physics_material.h"
#include "core/physics/physics_geometry.h"
#include "core/physics/physics_box_geometry.h"
#include "core/physics/physics_sphere_geometry.h"
#include "core/physics/physics_capsule_geometry.h"
#include "core/physics/physics_triangle_mesh_geometry.h"
#include "components/physics/rigidbody_dynamic.h"
#include "components/physics/rigidbody_static.h"
#include "components/physics/character_controller.h"
#include "components/physics/physics_static_plane.h"
#include "components/physics/trigger_collider.h"

#include "components/audio/audio_source.h"
#include "core/audio/audio_clip.h"
#include "components/audio/audio_listener.h"

#include "core/resources/model_loader.h"

#include "core/networking/packet_factory.h"
#include "core/networking/network_manager.h"
#include "core/networking/game_data_manager.h"
#include "core/networking/peer.h"
#include "core/networking/peer_factory.h"
#include "core/networking/packet_identifiers.h"
#include "core/networking/packet_handler.h"
#include "core/networking/packet_receiver.h"
#include "core/networking/serializable_component_types.h"
#include "core/networking/packet_receiver.h"
#include "core/networking/i_network_event_handler.h"
#include "core/networking/i_network_object_creator.h"
#include "core/networking/player_connectivity_data.h"
#include "components/networking/transform_serialization_component.h"