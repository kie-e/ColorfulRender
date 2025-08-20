import cv2
import pygame

image_path = input('Filename: ')
_9bit_mode = input('9 or 10-bit mode: ') == '9' # Reduces bit depth but also reduces flickering

if _9bit_mode:
    print('(Converting into 9-bit format.)')
else:
    print('(Converting into 10-bit format.)')

pygame.init()
image = cv2.imread(image_path, cv2.IMREAD_UNCHANGED)

alright = False

if image is not None:
    if image.dtype == cv2.numpy.uint16:
        alright = True
    else:
        print(f"Your image is not 16-bit.")
else:
    print(f"Failed to load image: {image_path}")

if alright:
    height, width, channels = image.shape

    main = pygame.Surface((width, height), pygame.SRCALPHA, 32)
    additional = pygame.Surface((width, height), pygame.SRCALPHA, 32)

    a = 1023
    for y in range(height):
        if y % 20 == 0:
            print(f'{y} / {height}')

        for x in range(width):
            # Convert from 16-bit to 10-bit first

            r = int(image[y][x][2] >> 6)
            g = int(image[y][x][1] >> 6)
            b = int(image[y][x][0] >> 6)

            if channels == 4:
                a = int(image[y][x][3] >> 6)

            # Then put the 8-bit part and 2-bit part in different images

            main.set_at((x, y), pygame.Color(r >> 2, g >> 2, b >> 2, a >> 2))

            if _9bit_mode:
                additional.set_at((x, y), pygame.Color((r % 4 >> 1) * 150, (g % 4 >> 1) * 150, (b % 4 >> 1) * 150, (a % 4 >> 1) * 150))
            else: # 10-bit mode
                additional.set_at((x, y), pygame.Color(r % 4 * 75, g % 4 * 75, b % 4 * 75, a % 4 * 75))

    pygame.image.save(main, 'cr.png')
    pygame.image.save(additional, 'cr_.png')