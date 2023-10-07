import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Make matplotlib look nicer
mpl.rc("lines", linewidth=0.8)
mpl.rc("axes", labelsize=4, titlesize=6, linewidth=0.2)
mpl.rc("figure", dpi=600)


def init_conds(arr_size: int = 15, seed_points=5) -> np.array:
    # Generate the initial square matrix to use as a seed.
    # There's no real reason for a square matrix other than simplicity.
    # Let's define 0=dead, 1=alive.

    start_matrix = np.zeros(shape=(arr_size, arr_size))
    n = 1
    while n < seed_points:
        # Using np.random_normal results in an array with a lot of live cells,
        # For better visualisation, use variable seed points
        i_rand = np.random.randint(low=0, high=arr_size)
        j_rand = np.random.randint(low=0, high=arr_size)
        start_matrix[i_rand, j_rand] = 1
        n += 1

    return start_matrix


def enforce_rules(array_in):
    array_out = np.zeros(array_in.shape)

    # I am taking an len(n-1) approach for simplicity.
    for i in range(1, array_in.shape[0] - 1):
        for j in range(1, array_in.shape[1] - 1):
            # Diagonals also need to be counted.
            top_left_val = array_in[i - 1, j + 1]
            top_val = array_in[i, j + 1]
            top_right_val = array_in[i + 1, j + 1]
            right_val = array_in[i + 1, j]
            bottom_right_val = array_in[i + 1, j - 1]
            bottom_val = array_in[i, j - 1]
            bottom_left_val = array_in[i - 1, j - 1]
            left_val = array_in[i - 1, j]

            total = (
                top_left_val
                + top_val
                + top_right_val
                + right_val
                + bottom_right_val
                + bottom_val
                + bottom_left_val
                + left_val
            )

            # Defining Conway's rules:
            # Rule 1: A live cell dies if it has fewer than two live neighbors.
            if (total < 2) and (array_in[i, j] == 1):
                array_out[i, j] = 0
            # Rule 2: A live cell with more than three live neighbors dies.
            elif (total > 3) and (array_in[i, j] == 1):
                array_out[i, j] = 0
            # Rule 3: A live cell with two or three live neighbors lives on to the next generation.
            elif (total == 2) or (total == 3) and (array_in[i, j] == 1):
                array_out[i, j] = 1
            # Rule 4: A dead cell will be brought back to live if it has exactly three live neighbors.
            elif (total == 3) and (array_in[i, j] == 0):
                array_out[i, j] = 1

    return array_out


def main(square_array_size, initial_alive_cells, timesteps_to_simulate):
    print(
        f"Generating initial conditions: {initial_alive_cells} initial cells, {square_array_size}x{square_array_size} grid.."
    )
    game_steps = []
    init_array = init_conds(arr_size=square_array_size, seed_points=initial_alive_cells)
    game_steps.append(init_array)

    print(f"Simulating t={timesteps_to_simulate}..")
    for i in range(1, timesteps_to_simulate):
        game_steps.append(enforce_rules(game_steps[i - 1]))

    fig = plt.figure(figsize=(4, 4))
    cmap = mpl.colors.ListedColormap(["white", "black"])
    im = plt.imshow(game_steps[0], cmap=cmap)

    plt.title(
        "Conway's game of life, standard rules, random initialisation.", fontsize=4
    )
    plt.xticks([], [])
    plt.yticks([], [])
    plt.get_current_fig_manager().window.state("zoomed")

    def updatefig(j):
        # set the data in the axesimage object
        im.set_array(game_steps[j])
        plt.xlabel(f"t={j}", fontsize=2)
        plt.xticks([], [])
        plt.yticks([], [])
        # return the artists set
        return [im]

    ani = animation.FuncAnimation(
        fig, updatefig, frames=range(20), interval=500, blit=False
    )
    plt.show()


if __name__ == "__main__":
    square_array_size = 150
    initial_alive_cells = 100
    timesteps_to_simulate = 250
    main(square_array_size, initial_alive_cells, timesteps_to_simulate)


# Some additional rules that could be cool to add:
# High Life: In High Life, a cell survives if it has 2 or 3 neighbors, like in the standard rules. However, it also comes to life (or stays alive) if it has 3 or 6 neighbors.

# Seeds: Seeds is a variation where every cell with 2 or 3 neighbors survives, but a cell with 4 neighbors comes to life.

# Day and Night: In Day and Night, cells with 3, 4, 6, or 7 neighbors survive, and dead cells with 3, 6, 7, or 8 neighbors come to life.

# Replicator: The Replicator is a pattern that, as the name suggests, replicates itself. In this variation, you can define custom rules where the pattern remains stable and replicates.

# Maze: The Maze variation aims to create maze-like patterns. Cells survive with 1, 2, 3, 4, or 5 neighbors, and dead cells come to life with exactly 3 neighbors.

# Coral: Coral is a variation where cells survive with 4, 5, 6, 7, or 8 neighbors but only come to life with exactly 3 neighbors.

# Maze Generators: You can implement rules that generate mazes using Conway's Game of Life. For example, you could use a rule where cells with 5 neighbors become walls, and cells with 3 neighbors become pathways.

# Custom Rules: Get creative and design your own custom rules. Experiment with different combinations of survival and birth conditions to create unique patterns and behaviors.
