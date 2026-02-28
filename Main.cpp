--[[
    Jitter.xyz Framework
    Expert Reverse Engineering Style Implementation
    Target: Xeno / Universal Roblox
]]

local UserInputService = game:GetService("UserInputService")
local TweenService = game:GetService("TweenService")
local RunService = game:GetService("RunService")
local LocalPlayer = game:GetService("Players").LocalPlayer
local Mouse = LocalPlayer:GetMouse()

local Jitter = {
    Active = true,
    CurrentTab = "Ragebot",
    UIElements = {},
    AccentColor = Color3.fromRGB(0, 162, 255), -- Характерный цвет Neverlose
}

-- Создание основы
local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name = "Jitter_Project"
ScreenGui.Parent = (gethui and gethui()) or game:GetService("CoreGui") -- Используем CoreGui или Hidden UI для обхода детекта обычными скриптами

-- Главное окно
local MainFrame = Instance.new("Frame")
MainFrame.Name = "Main"
MainFrame.Size = UDim2.new(0, 750, 0, 500)
MainFrame.Position = UDim2.new(0.5, -375, 0.5, -250)
MainFrame.BackgroundColor3 = Color3.fromRGB(11, 14, 19)
MainFrame.BorderSizePixel = 0
MainFrame.Parent = ScreenGui

-- Скругление и обводка (Glow эффект)
local UICorner = Instance.new("UICorner")
UICorner.CornerRadius = UDim.new(0, 4)
UICorner.Parent = MainFrame

local UIStroke = Instance.new("UIStroke")
UIStroke.Color = Jitter.AccentColor
UIStroke.Thickness = 1.5
UIStroke.Transparency = 0.8
UIStroke.Parent = MainFrame

-- Сайдбар
local Sidebar = Instance.new("Frame")
Sidebar.Size = UDim2.new(0, 180, 1, 0)
Sidebar.BackgroundColor3 = Color3.fromRGB(8, 11, 15)
Sidebar.BorderSizePixel = 0
Sidebar.Parent = MainFrame

-- Логотип Jitter.xyz
local Logo = Instance.new("TextLabel")
Logo.Text = "JITTER.XYZ"
Logo.Font = Enum.Font.GothamBlack
Logo.TextSize = 20
Logo.TextColor3 = Color3.fromRGB(255, 255, 255)
Logo.Position = UDim2.new(0, 20, 0, 20)
Logo.Size = UDim2.new(0, 140, 0, 40)
Logo.BackgroundTransparency = 1
Logo.TextXAlignment = Enum.TextXAlignment.Left
Logo.Parent = Sidebar

-- Контейнер контента
local Container = Instance.new("Frame")
Container.Name = "Container"
Container.Size = UDim2.new(1, -190, 1, -20)
Container.Position = UDim2.new(0, 190, 0, 10)
Container.BackgroundTransparency = 1
Container.Parent = MainFrame

-- Утилита для создания вкладок
local function CreateTabButton(name, icon)
    local TabBtn = Instance.new("TextButton")
    TabBtn.Size = UDim2.new(1, -20, 0, 35)
    TabBtn.Position = UDim2.new(0, 10, 0, 70 + (#Sidebar:GetChildren() - 2) * 40)
    TabBtn.BackgroundColor3 = Color3.fromRGB(20, 25, 35)
    TabBtn.BackgroundTransparency = 1
    TabBtn.Text = "  " .. name
    TabBtn.Font = Enum.Font.GothamSemibold
    TabBtn.TextColor3 = Color3.fromRGB(150, 150, 160)
    TabBtn.TextSize = 14
    TabBtn.TextXAlignment = Enum.TextXAlignment.Left
    TabBtn.Parent = Sidebar

    TabBtn.MouseButton1Click:Connect(function()
        Jitter.CurrentTab = name
        -- Здесь логика переключения фреймов
        print("Switched to: " .. name)
    end)
end

-- Наполнение (по скриншоту Neverlose)
CreateTabButton("Ragebot")
CreateTabButton("Anti Aim")
CreateTabButton("Legitbot")
CreateTabButton("Players")
CreateTabButton("World")
CreateTabButton("Skins")
CreateTabButton("Settings")

-- Сетка (Sections)
local function CreateSection(title, pos, size)
    local Section = Instance.new("Frame")
    Section.Size = size
    Section.Position = pos
    Section.BackgroundColor3 = Color3.fromRGB(14, 18, 24)
    Section.BorderSizePixel = 0
    Section.Parent = Container

    local SCorner = Instance.new("UICorner")
    SCorner.CornerRadius = UDim.new(0, 4)
    SCorner.Parent = Section

    local STitle = Instance.new("TextLabel")
    STitle.Text = title:upper()
    STitle.Font = Enum.Font.GothamBold
    STitle.TextSize = 12
    STitle.TextColor3 = Color3.fromRGB(200, 200, 210)
    STitle.Position = UDim2.new(0, 10, 0, 10)
    STitle.BackgroundTransparency = 1
    STitle.Parent = Section
    
    return Section
end

-- Пример: Секция Main в Ragebot
local MainSection = CreateSection("Main", UDim2.new(0, 5, 0, 5), UDim2.new(0, 260, 0, 230))

-- Логика Drag & Drop (Перетаскивание)
local dragging, dragInput, dragStart, startPos
MainFrame.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 then
        dragging = true
        dragStart = input.Position
        startPos = MainFrame.Position
    end
end)

UserInputService.InputChanged:Connect(function(input)
    if dragging and input.UserInputType == Enum.UserInputType.MouseMovement then
        local delta = input.Position - dragStart
        MainFrame.Position = UDim2.new(startPos.X.Scale, startPos.X.Offset + delta.X, startPos.Y.Scale, startPos.Y.Offset + delta.Y)
    end
end)

MainFrame.InputEnded:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 then
        dragging = false
    end
end)

-- Toggle Hide/Show (Insert)
UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.Insert then
        ScreenGui.Enabled = not ScreenGui.Enabled
    end
end)

print("Jitter.xyz Loaded. Press INSERT to toggle.")
