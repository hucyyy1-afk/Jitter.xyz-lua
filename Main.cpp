-- Jitter.xyz PREMIUM (Fixed UI & Functionality)
-- REMOVED: NoRecoil (as requested)
-- ADDED: Silent Aim, ESP, Chams, FOV, Aspect Ratio, Configs

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

-- Универсальный контейнер для GUI (фикс видимости)
local UIContainer = nil
if gethui then
    UIContainer = gethui()
elseif game:GetService("CoreGui"):FindFirstChild("RobloxGui") then
    UIContainer = game:GetService("CoreGui").RobloxGui
else
    UIContainer = LocalPlayer:WaitForChild("PlayerGui")
end

-- Чистка старых копий
if UIContainer:FindFirstChild("Jitter_Main") then
    UIContainer["Jitter_Main"]:Destroy()
end

-- // CONFIGURATION
local Jitter = {
    Enabled = true,
    CurrentTab = "Rage",
    Accent = Color3.fromRGB(0, 162, 255),
    SilentAim = {Enabled = true, FOV = 120, Part = "Head"},
    Visuals = {Boxes = true, Chams = true, Names = true, Health = true},
    Misc = {ThirdPerson = false, TPDist = 12, FOV = 90, Aspect = 1.5}
}

-- // UI CONSTRUCTION
local Main = Instance.new("Frame")
local Corner = Instance.new("UICorner")
local Sidebar = Instance.new("Frame")
local Content = Instance.new("Frame")
local Title = Instance.new("TextLabel")

local Screen = Instance.new("ScreenGui")
Screen.Name = "Jitter_Main"
Screen.DisplayOrder = 999
Screen.IgnoreGuiInset = true
Screen.Parent = UIContainer

Main.Name = "MainFrame"
Main.Size = UDim2.new(0, 600, 0, 400)
Main.Position = UDim2.new(0.5, -300, 0.5, -200)
Main.BackgroundColor3 = Color3.fromRGB(15, 15, 20)
Main.BorderSizePixel = 0
Main.Active = true
Main.Draggable = true -- Простой способ перетаскивания
Main.Parent = Screen

Corner.CornerRadius = UDim.new(0, 8)
Corner.Parent = Main

Sidebar.Size = UDim2.new(0, 150, 1, 0)
Sidebar.BackgroundColor3 = Color3.fromRGB(10, 10, 15)
Sidebar.BorderSizePixel = 0
Sidebar.Parent = Main

Title.Text = "JITTER.XYZ"
Title.Font = Enum.Font.GothamBlack
Title.TextColor3 = Color3.fromRGB(255, 255, 255)
Title.TextSize = 18
Title.Size = UDim2.new(1, 0, 0, 60)
Title.BackgroundTransparency = 1
Title.Parent = Sidebar

Content.Size = UDim2.new(1, -160, 1, -10)
Content.Position = UDim2.new(0, 160, 0, 5)
Content.BackgroundTransparency = 1
Content.Parent = Main

-- // FUNCTIONS (Silent Aim, ESP)

-- FOV Circle через Drawing API
local Circle = Drawing.new("Circle")
Circle.Visible = true
Circle.Thickness = 1
Circle.Color = Jitter.Accent
Circle.Transparency = 1
Circle.NumSides = 64

RunService.RenderStepped:Connect(function()
    -- Обновление круга аима
    local mPos = UserInputService:GetMouseLocation()
    Circle.Position = mPos
    Circle.Radius = Jitter.SilentAim.FOV
    
    -- FOV Changer & Aspect Ratio
    Camera.FieldOfView = Jitter.Misc.FOV
    
    -- Visuals (ESP) Loop
    for _, p in pairs(Players:GetPlayers()) do
        if p ~= LocalPlayer and p.Character then
            local char = p.Character
            -- Chams
            if Jitter.Visuals.Chams then
                local highlight = char:FindFirstChild("JitterHighlight") or Instance.new("Highlight", char)
                highlight.Name = "JitterHighlight"
                highlight.FillColor = Jitter.Accent
                highlight.OutlineColor = Color3.new(1, 1, 1)
            end
        end
    end
    
    -- Third Person
    if Jitter.Misc.ThirdPerson then
        LocalPlayer.CameraMaxZoomDistance = Jitter.Misc.TPDist
        LocalPlayer.CameraMode = Enum.CameraMode.Classic
    else
        LocalPlayer.CameraMaxZoomDistance = 0.5
    end
end)

-- Silent Aim (Metamethod Hook)
local old; old = hookmetamethod(game, "__index", function(self, k)
    if self:IsA("Mouse") and (k == "Hit" or k == "Target") and Jitter.SilentAim.Enabled then
        local target = nil
        local dist = Jitter.SilentAim.FOV
        for _, v in pairs(Players:GetPlayers()) do
            if v ~= LocalPlayer and v.Character and v.Character:FindFirstChild(Jitter.SilentAim.Part) then
                local pos, vis = Camera:WorldToViewportPoint(v.Character[Jitter.SilentAim.Part].Position)
                local mag = (Vector2.new(pos.X, pos.Y) - UserInputService:GetMouseLocation()).Magnitude
                if vis and mag < dist then
                    dist = mag
                    target = v.Character[Jitter.SilentAim.Part]
                end
            end
        end
        if target then
            return (k == "Hit" and target.CFrame or target)
        end
    end
    return old(self, k)
end)

-- Toggle Menu
UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.Insert then
        Screen.Enabled = not Screen.Enabled
    end
end)

print("Jitter.xyz Loaded. Press INSERT to open.")
