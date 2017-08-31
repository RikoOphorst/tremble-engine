/********************************************************************************
** Form generated from reading UI file 'config_design.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef CONFIG_DESIGN_H
#define CONFIG_DESIGN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetupDialog
{
public:
    QWidget *root;
    QWidget *formLayoutWidget;
    QFormLayout *form_layout;
    QLabel *label_fullscreen;
    QCheckBox *is_fullscreen;
    QLabel *label_gpu;
    QComboBox *selection_gpu;
    QLabel *label_window_resolution;
    QComboBox *selection_window_resolution;
    QLabel *label_render_resolution;
    QComboBox *selection_render_resolution;
    QLabel *label_scene;
    QComboBox *selection_scene;
    QLabel *label_load_scene;
    QCheckBox *is_load_scene;
    QLabel *label_god_mode;
    QCheckBox *is_god_mode;
    QLabel *label_frustum_culling;
    QCheckBox *is_frustum_culling;
    QLabel *label_depth_pre_pass;
    QCheckBox *is_depth_pre_pass;
    QLabel *label_physics_base_plane;
    QCheckBox *is_physics_base_plane;
    QLabel *label_d3d12_debug_layer;
    QCheckBox *is_d3d12_debug_layer;
    QLabel *label_wireframe;
    QCheckBox *is_wireframe;
    QLabel *label_light_demo;
    QCheckBox *is_light_demo;
    QLabel *label_num_lights;
    QSpinBox *num_lights;
    QLabel *label_master_volume;
    QSlider *slider_master_volume;
    QLabel *label_mouse_sensitivity;
    QSlider *slider_mouse_sensitivity;
    QLabel *label_host;
    QCheckBox *is_host;
    QLabel *label_host_ip;
    QLineEdit *input_host_ip_address;
    QLabel *label_my_ip;
    QLineEdit *input_my_ip_address;
    QLabel *label_nickname;
    QLineEdit *input_nickname;
    QDialogButtonBox *launch_box;
    QCheckBox *is_spawn_clients;
    QLabel *label_spawn_clients;
    QSpinBox *num_clients;
    QLabel *label_num_clients;

    void setupUi(QMainWindow *SetupDialog)
    {
        if (SetupDialog->objectName().isEmpty())
            SetupDialog->setObjectName(QStringLiteral("SetupDialog"));
        SetupDialog->resize(461, 743);
        SetupDialog->setMinimumSize(QSize(461, 300));
        SetupDialog->setMaximumSize(QSize(461, 2000));
        SetupDialog->setBaseSize(QSize(461, 500));
        QFont font;
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        font.setStyleStrategy(QFont::PreferDefault);
        SetupDialog->setFont(font);
        root = new QWidget(SetupDialog);
        root->setObjectName(QStringLiteral("root"));
        formLayoutWidget = new QWidget(root);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(0, 0, 461, 741));
        form_layout = new QFormLayout(formLayoutWidget);
        form_layout->setObjectName(QStringLiteral("form_layout"));
        form_layout->setRowWrapPolicy(QFormLayout::WrapLongRows);
        form_layout->setHorizontalSpacing(70);
        form_layout->setVerticalSpacing(15);
        form_layout->setContentsMargins(20, 20, 20, 0);
        label_fullscreen = new QLabel(formLayoutWidget);
        label_fullscreen->setObjectName(QStringLiteral("label_fullscreen"));

        form_layout->setWidget(0, QFormLayout::LabelRole, label_fullscreen);

        is_fullscreen = new QCheckBox(formLayoutWidget);
        is_fullscreen->setObjectName(QStringLiteral("is_fullscreen"));

        form_layout->setWidget(0, QFormLayout::FieldRole, is_fullscreen);

        label_gpu = new QLabel(formLayoutWidget);
        label_gpu->setObjectName(QStringLiteral("label_gpu"));

        form_layout->setWidget(1, QFormLayout::LabelRole, label_gpu);

        selection_gpu = new QComboBox(formLayoutWidget);
        selection_gpu->setObjectName(QStringLiteral("selection_gpu"));

        form_layout->setWidget(1, QFormLayout::FieldRole, selection_gpu);

        label_window_resolution = new QLabel(formLayoutWidget);
        label_window_resolution->setObjectName(QStringLiteral("label_window_resolution"));

        form_layout->setWidget(2, QFormLayout::LabelRole, label_window_resolution);

        selection_window_resolution = new QComboBox(formLayoutWidget);
        selection_window_resolution->setObjectName(QStringLiteral("selection_window_resolution"));
        selection_window_resolution->setEnabled(true);

        form_layout->setWidget(2, QFormLayout::FieldRole, selection_window_resolution);

        label_render_resolution = new QLabel(formLayoutWidget);
        label_render_resolution->setObjectName(QStringLiteral("label_render_resolution"));

        form_layout->setWidget(3, QFormLayout::LabelRole, label_render_resolution);

        selection_render_resolution = new QComboBox(formLayoutWidget);
        selection_render_resolution->setObjectName(QStringLiteral("selection_render_resolution"));

        form_layout->setWidget(3, QFormLayout::FieldRole, selection_render_resolution);

        label_scene = new QLabel(formLayoutWidget);
        label_scene->setObjectName(QStringLiteral("label_scene"));

        form_layout->setWidget(4, QFormLayout::LabelRole, label_scene);

        selection_scene = new QComboBox(formLayoutWidget);
        selection_scene->setObjectName(QStringLiteral("selection_scene"));

        form_layout->setWidget(4, QFormLayout::FieldRole, selection_scene);

        label_load_scene = new QLabel(formLayoutWidget);
        label_load_scene->setObjectName(QStringLiteral("label_load_scene"));

        form_layout->setWidget(5, QFormLayout::LabelRole, label_load_scene);

        is_load_scene = new QCheckBox(formLayoutWidget);
        is_load_scene->setObjectName(QStringLiteral("is_load_scene"));
        is_load_scene->setChecked(true);

        form_layout->setWidget(5, QFormLayout::FieldRole, is_load_scene);

        label_god_mode = new QLabel(formLayoutWidget);
        label_god_mode->setObjectName(QStringLiteral("label_god_mode"));

        form_layout->setWidget(6, QFormLayout::LabelRole, label_god_mode);

        is_god_mode = new QCheckBox(formLayoutWidget);
        is_god_mode->setObjectName(QStringLiteral("is_god_mode"));

        form_layout->setWidget(6, QFormLayout::FieldRole, is_god_mode);

        label_frustum_culling = new QLabel(formLayoutWidget);
        label_frustum_culling->setObjectName(QStringLiteral("label_frustum_culling"));

        form_layout->setWidget(7, QFormLayout::LabelRole, label_frustum_culling);

        is_frustum_culling = new QCheckBox(formLayoutWidget);
        is_frustum_culling->setObjectName(QStringLiteral("is_frustum_culling"));
        is_frustum_culling->setEnabled(true);
        is_frustum_culling->setChecked(true);
        is_frustum_culling->setAutoRepeat(false);

        form_layout->setWidget(7, QFormLayout::FieldRole, is_frustum_culling);

        label_depth_pre_pass = new QLabel(formLayoutWidget);
        label_depth_pre_pass->setObjectName(QStringLiteral("label_depth_pre_pass"));

        form_layout->setWidget(8, QFormLayout::LabelRole, label_depth_pre_pass);

        is_depth_pre_pass = new QCheckBox(formLayoutWidget);
        is_depth_pre_pass->setObjectName(QStringLiteral("is_depth_pre_pass"));
        is_depth_pre_pass->setEnabled(true);
        is_depth_pre_pass->setChecked(true);
        is_depth_pre_pass->setAutoRepeat(false);

        form_layout->setWidget(8, QFormLayout::FieldRole, is_depth_pre_pass);

        label_physics_base_plane = new QLabel(formLayoutWidget);
        label_physics_base_plane->setObjectName(QStringLiteral("label_physics_base_plane"));

        form_layout->setWidget(9, QFormLayout::LabelRole, label_physics_base_plane);

        is_physics_base_plane = new QCheckBox(formLayoutWidget);
        is_physics_base_plane->setObjectName(QStringLiteral("is_physics_base_plane"));
        is_physics_base_plane->setEnabled(true);
        is_physics_base_plane->setChecked(false);
        is_physics_base_plane->setAutoRepeat(false);

        form_layout->setWidget(9, QFormLayout::FieldRole, is_physics_base_plane);

        label_d3d12_debug_layer = new QLabel(formLayoutWidget);
        label_d3d12_debug_layer->setObjectName(QStringLiteral("label_d3d12_debug_layer"));

        form_layout->setWidget(10, QFormLayout::LabelRole, label_d3d12_debug_layer);

        is_d3d12_debug_layer = new QCheckBox(formLayoutWidget);
        is_d3d12_debug_layer->setObjectName(QStringLiteral("is_d3d12_debug_layer"));
        is_d3d12_debug_layer->setEnabled(true);
        is_d3d12_debug_layer->setChecked(false);
        is_d3d12_debug_layer->setAutoRepeat(false);

        form_layout->setWidget(10, QFormLayout::FieldRole, is_d3d12_debug_layer);

        label_wireframe = new QLabel(formLayoutWidget);
        label_wireframe->setObjectName(QStringLiteral("label_wireframe"));

        form_layout->setWidget(11, QFormLayout::LabelRole, label_wireframe);

        is_wireframe = new QCheckBox(formLayoutWidget);
        is_wireframe->setObjectName(QStringLiteral("is_wireframe"));
        is_wireframe->setEnabled(true);
        is_wireframe->setChecked(false);
        is_wireframe->setAutoRepeat(false);

        form_layout->setWidget(11, QFormLayout::FieldRole, is_wireframe);

        label_light_demo = new QLabel(formLayoutWidget);
        label_light_demo->setObjectName(QStringLiteral("label_light_demo"));

        form_layout->setWidget(12, QFormLayout::LabelRole, label_light_demo);

        is_light_demo = new QCheckBox(formLayoutWidget);
        is_light_demo->setObjectName(QStringLiteral("is_light_demo"));
        is_light_demo->setEnabled(true);
        is_light_demo->setChecked(false);
        is_light_demo->setAutoRepeat(false);

        form_layout->setWidget(12, QFormLayout::FieldRole, is_light_demo);

        label_num_lights = new QLabel(formLayoutWidget);
        label_num_lights->setObjectName(QStringLiteral("label_num_lights"));

        form_layout->setWidget(13, QFormLayout::LabelRole, label_num_lights);

        num_lights = new QSpinBox(formLayoutWidget);
        num_lights->setObjectName(QStringLiteral("num_lights"));
        num_lights->setReadOnly(false);
        num_lights->setMaximum(64);

        form_layout->setWidget(13, QFormLayout::FieldRole, num_lights);

        label_master_volume = new QLabel(formLayoutWidget);
        label_master_volume->setObjectName(QStringLiteral("label_master_volume"));

        form_layout->setWidget(14, QFormLayout::LabelRole, label_master_volume);

        slider_master_volume = new QSlider(formLayoutWidget);
        slider_master_volume->setObjectName(QStringLiteral("slider_master_volume"));
        slider_master_volume->setMaximum(100);
        slider_master_volume->setOrientation(Qt::Horizontal);

        form_layout->setWidget(14, QFormLayout::FieldRole, slider_master_volume);

        label_mouse_sensitivity = new QLabel(formLayoutWidget);
        label_mouse_sensitivity->setObjectName(QStringLiteral("label_mouse_sensitivity"));

        form_layout->setWidget(15, QFormLayout::LabelRole, label_mouse_sensitivity);

        slider_mouse_sensitivity = new QSlider(formLayoutWidget);
        slider_mouse_sensitivity->setObjectName(QStringLiteral("slider_mouse_sensitivity"));
        slider_mouse_sensitivity->setMaximum(100);
        slider_mouse_sensitivity->setOrientation(Qt::Horizontal);

        form_layout->setWidget(15, QFormLayout::FieldRole, slider_mouse_sensitivity);

        label_host = new QLabel(formLayoutWidget);
        label_host->setObjectName(QStringLiteral("label_host"));

        form_layout->setWidget(16, QFormLayout::LabelRole, label_host);

        is_host = new QCheckBox(formLayoutWidget);
        is_host->setObjectName(QStringLiteral("is_host"));
        is_host->setEnabled(true);
        is_host->setChecked(true);
        is_host->setAutoRepeat(false);

        form_layout->setWidget(16, QFormLayout::FieldRole, is_host);

        label_host_ip = new QLabel(formLayoutWidget);
        label_host_ip->setObjectName(QStringLiteral("label_host_ip"));

        form_layout->setWidget(17, QFormLayout::LabelRole, label_host_ip);

        input_host_ip_address = new QLineEdit(formLayoutWidget);
        input_host_ip_address->setObjectName(QStringLiteral("input_host_ip_address"));

        form_layout->setWidget(17, QFormLayout::FieldRole, input_host_ip_address);

        label_my_ip = new QLabel(formLayoutWidget);
        label_my_ip->setObjectName(QStringLiteral("label_my_ip"));

        form_layout->setWidget(18, QFormLayout::LabelRole, label_my_ip);

        input_my_ip_address = new QLineEdit(formLayoutWidget);
        input_my_ip_address->setObjectName(QStringLiteral("input_my_ip_address"));
        input_my_ip_address->setReadOnly(true);

        form_layout->setWidget(18, QFormLayout::FieldRole, input_my_ip_address);

        label_nickname = new QLabel(formLayoutWidget);
        label_nickname->setObjectName(QStringLiteral("label_nickname"));

        form_layout->setWidget(19, QFormLayout::LabelRole, label_nickname);

        input_nickname = new QLineEdit(formLayoutWidget);
        input_nickname->setObjectName(QStringLiteral("input_nickname"));
        input_nickname->setReadOnly(false);

        form_layout->setWidget(19, QFormLayout::FieldRole, input_nickname);

        launch_box = new QDialogButtonBox(formLayoutWidget);
        launch_box->setObjectName(QStringLiteral("launch_box"));
        launch_box->setOrientation(Qt::Horizontal);
        launch_box->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        launch_box->setCenterButtons(false);

        form_layout->setWidget(22, QFormLayout::FieldRole, launch_box);

        is_spawn_clients = new QCheckBox(formLayoutWidget);
        is_spawn_clients->setObjectName(QStringLiteral("is_spawn_clients"));
        is_spawn_clients->setEnabled(true);
        is_spawn_clients->setChecked(false);
        is_spawn_clients->setAutoRepeat(false);

        form_layout->setWidget(20, QFormLayout::FieldRole, is_spawn_clients);

        label_spawn_clients = new QLabel(formLayoutWidget);
        label_spawn_clients->setObjectName(QStringLiteral("label_spawn_clients"));

        form_layout->setWidget(20, QFormLayout::LabelRole, label_spawn_clients);

        num_clients = new QSpinBox(formLayoutWidget);
        num_clients->setObjectName(QStringLiteral("num_clients"));
        num_clients->setReadOnly(false);
        num_clients->setMaximum(99);

        form_layout->setWidget(21, QFormLayout::FieldRole, num_clients);

        label_num_clients = new QLabel(formLayoutWidget);
        label_num_clients->setObjectName(QStringLiteral("label_num_clients"));

        form_layout->setWidget(21, QFormLayout::LabelRole, label_num_clients);

        SetupDialog->setCentralWidget(root);

        retranslateUi(SetupDialog);

        QMetaObject::connectSlotsByName(SetupDialog);
    } // setupUi

    void retranslateUi(QMainWindow *SetupDialog)
    {
        SetupDialog->setWindowTitle(QApplication::translate("SetupDialog", "Runtime setup", 0));
        label_fullscreen->setText(QApplication::translate("SetupDialog", "Full screen", 0));
        is_fullscreen->setText(QString());
        label_gpu->setText(QApplication::translate("SetupDialog", "GPU", 0));
        label_window_resolution->setText(QApplication::translate("SetupDialog", "Window resolution", 0));
        label_render_resolution->setText(QApplication::translate("SetupDialog", "Render resolution", 0));
        label_scene->setText(QApplication::translate("SetupDialog", "Scene", 0));
        label_load_scene->setText(QApplication::translate("SetupDialog", "Load Scene", 0));
        is_load_scene->setText(QString());
        label_god_mode->setText(QApplication::translate("SetupDialog", "God Mode", 0));
        is_god_mode->setText(QString());
        label_frustum_culling->setText(QApplication::translate("SetupDialog", "Frustum culling", 0));
        is_frustum_culling->setText(QString());
        label_depth_pre_pass->setText(QApplication::translate("SetupDialog", "Depth Pre-Pass", 0));
        is_depth_pre_pass->setText(QString());
        label_physics_base_plane->setText(QApplication::translate("SetupDialog", "Physics base plane", 0));
        is_physics_base_plane->setText(QString());
        label_d3d12_debug_layer->setText(QApplication::translate("SetupDialog", "D3D12 Debug Layer", 0));
        is_d3d12_debug_layer->setText(QString());
        label_wireframe->setText(QApplication::translate("SetupDialog", "Wireframe rendering", 0));
        is_wireframe->setText(QString());
        label_light_demo->setText(QApplication::translate("SetupDialog", "Light demo", 0));
        is_light_demo->setText(QString());
        label_num_lights->setText(QApplication::translate("SetupDialog", "# of lights in demo", 0));
        label_master_volume->setText(QApplication::translate("SetupDialog", "Master volume", 0));
        label_mouse_sensitivity->setText(QApplication::translate("SetupDialog", "Mouse sensitivity", 0));
        label_host->setText(QApplication::translate("SetupDialog", "Host", 0));
        is_host->setText(QString());
        label_host_ip->setText(QApplication::translate("SetupDialog", "Host IP address", 0));
        input_host_ip_address->setPlaceholderText(QApplication::translate("SetupDialog", "127.0.0.1", 0));
        label_my_ip->setText(QApplication::translate("SetupDialog", "My IP address", 0));
        input_my_ip_address->setPlaceholderText(QApplication::translate("SetupDialog", "127.0.0.1", 0));
        label_nickname->setText(QApplication::translate("SetupDialog", "Nickname", 0));
        input_nickname->setPlaceholderText(QApplication::translate("SetupDialog", "Optic[S0_L3g1t]", 0));
        is_spawn_clients->setText(QString());
        label_spawn_clients->setText(QApplication::translate("SetupDialog", "Spawn clients", 0));
        label_num_clients->setText(QApplication::translate("SetupDialog", "# of clients", 0));
    } // retranslateUi

};

namespace Ui {
    class SetupDialog: public Ui_SetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CONFIG_DESIGN_H
