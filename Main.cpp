-- Jitter.xyz PREMIUM (Fixed Interaction & Box ESP)
-- Target: Xeno / Roblox 2026

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

local Jitter = {
    Enabled = true,
    Visuals = { Boxes = true, Chams = true, Tracers = true },
    SilentAim = { Enabled = true, FOV = 150, Part = "Head" },
    Accent = Color3.fromRGB(0, 162, 255)
}

-- Хранилище для отрисовки ESP
local ESP_Objects = {}

-- Функция создания ESP для игрока
local function CreateESP(player)
    local box = Drawing.new("Square")
    box.Visible = false
    box.Color = Jitter.Accent
    box.Thickness = 1
    box.Filled = false
    
    ESP_Objects[player] = { Box = box }
end

-- Инициализация ESP для текущих и новых игроков
for _, p in pairs(Players:GetPlayers()) do if p ~= LocalPlayer then CreateESP(p) end end
Players.PlayerAdded:Connect(function(p) if p ~= LocalPlayer then CreateESP(p) end end)

-- // UI CONSTRUCTION (Оживление меню)
local Screen = Instance.new("ScreenGui")
Screen.Name = "Jitter_Interactive"
Screen.DisplayOrder = 999
Screen.ZIndexBehavior = Enum.ZIndexBehavior.Sibling
Screen.Parent = (gethui and gethui()) or game:GetService("CoreGui")

local Main = Instance.new("Frame")
Main.Size = UDim2.new(0, 500, 0, 350)
Main.Position = UDim2.new(0.5, -250, 0.5, -175)
Main.BackgroundColor3 = Color3.fromRGB(15, 15, 20)
Main.Active = true
Main.Draggable = true -- Включено для удобства
Main.Parent = Screen

-- Заголовок (кнопка переключения вкладок для теста)
local TabBtn = Instance.new("TextButton")
TabBtn.Size = UDim2.new(0, 120, 0, 30)
TabBtn.Position = UDim2.new(0, 10, 0, 10)
TabBtn.BackgroundColor3 = Jitter.Accent
TabBtn.Text = "Toggle Boxes"
TabBtn.Font = Enum.Font.GothamBold
TabBtn.TextColor3 = Color3.new(1,1,1)
TabBtn.Parent = Main

TabBtn.MouseButton1Click:Connect(function()
    Jitter.Visuals.Boxes = not Jitter.Visuals.Boxes
    TabBtn.BackgroundColor3 = Jitter.Visuals.Boxes and Jitter.Accent or Color3.fromRGB(50, 50, 50)
    print("Boxes toggled: ", Jitter.Visuals.Boxes)
end)

-- // ГЛАВНЫЙ ЦИКЛ ОБНОВЛЕНИЯ (ESP & Visuals)
RunService.RenderStepped:Connect(function()
    for player, objects in pairs(ESP_Objects) do
        if player.Character and player.Character:FindFirstChild("HumanoidRootPart") and player.Character:FindFirstChild("Humanoid") then
            local char = player.Character
            local hrp = char.HumanoidRootPart
            local hum = char.Humanoid
            
            local pos, onScreen = Camera:WorldToViewportPoint(hrp.Position)
            
            if onScreen and Jitter.Visuals.Boxes and hum.Health > 0 then
                -- Расчет размера бокса в зависимости от дистанции
                local sizeX = 2000 / pos.Z
                local sizeY = 3000 / pos.Z
                
                objects.Box.Visible = true
                objects.Box.Size = Vector2.new(sizeX, sizeY)
                objects.Box.Position = Vector2.new(pos.X - sizeX/2, pos.Y - sizeY/2)
            else
                objects.Box.Visible = false
            end
        else
            objects.Box.Visible = false
        end
    end
end)

-- Управление мышью (чтобы меню не мешало кликать в игре, когда закрыто)
UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.Insert then
        Screen.Enabled = not Screen.Enabled
        UserInputService.MouseIconEnabled = Screen.Enabled -- Видим мышь только когда открыто меню
    end
end)

print("Jitter.xyz: Interactive Version Loaded. Press INSERT.")
